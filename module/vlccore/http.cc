// vlccore/http.cc
// See: vlc/modules/access/http.c
// 2/21/2012
// Icy, Proxy, and Authentification supports are not implemented.

#include "module/vlccore/http.h"
#include "module/datastream/bufferedremotestream.h"
#include <QtCore>
#include <QtNetwork>
#include <cstring>

#include "module/vlccore/types/module.h"

//#define DEBUG "vlccore::http"
#include "module/debug/debug.h"

// - Modules -

//static ssize_t Read(access_t *, uint8_t *, size_t);
//static ssize_t ReadCompressed(access_t *, uint8_t *, size_t);
//static int Seek(access_t *, uint64_t);
//static int Control(access_t *, int, va_list);

// - Session -

VlcHttpSession::VlcHttpSession(QObject *parent)
  : Base(parent), state_(Stopped), in_(0)
{
  in_ = new BufferedRemoteStream(this);
  if (VlcHttpPlugin::cookieJar())
    in_->networkAccessManager()->setCookieJar(VlcHttpPlugin::cookieJar());

  connect(in_, SIGNAL(error(QString)), SLOT(abort()));
  connect(in_, SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(in_, SIGNAL(finished()), SLOT(finish()));
  connect(in_, SIGNAL(readyRead()), SIGNAL(readyRead()));
}

VlcHttpSession::~VlcHttpSession()
{
  if (in_)
    delete in_;
}

void
VlcHttpSession::abort()
{
  DOUT("enter");
  stop();
  state_ = Error;

  DOUT("exit");
}

void
VlcHttpSession::finish()
{
  DOUT("enter");
  state_ = Finished;
  emit finished();
  DOUT("exit");
}

void
VlcHttpSession::stop()
{
  DOUT("enter");
  if (in_)
    in_->stop();
  state_ = Stopped;
  emit stopped();
  DOUT("exit");
}

void
VlcHttpSession::run()
{
  DOUT("enter");
  in_->setUrl(url_);
  in_->run();
  state_ = Running;
  DOUT("exit");
}

qint64
VlcHttpSession::read(char *data, qint64 maxSize)
{ return in_ ? in_->read(data, maxSize) : 0; }

QString
VlcHttpSession::contentType() const
{ return in_ ? in_->contentType() : QString(); }

qint64
VlcHttpSession::pos() const
{ return in_ ? in_->pos() : 0; }

qint64
VlcHttpSession::size() const
{ return in_ ? in_->size() : 0; }

bool
VlcHttpSession::seek(qint64 pos)
{ return in_ ? in_->seek(pos) : 0; }

// - Plugin -

QNetworkCookieJar *VlcHttpPlugin::cookieJar_ = 0;

void
VlcHttpPlugin::load()
{
#ifdef USE_MODULE_VLCCORE_HTTP
  module_t *m = ::module_find("access_http");
  Q_ASSERT(m);
  if (m && !m->b_loaded) {
    m->b_loaded = true;
    m->b_unloadable = false;
    m->pf_activate = open;
    m->pf_deactivate = close;
  }
#endif // USE_MODULE_VLCCORE_HTTP
}

void
VlcHttpPlugin::unload()
{
#ifdef USE_MODULE_VLCCORE_HTTP
  module_t *m = ::module_find("access_http");
  Q_ASSERT(m);
  if (m && m->b_loaded) {
    m->pf_activate = 0;
    m->pf_deactivate = 0;
    m->b_loaded = false;
    m->b_unloadable = true;
  }
#endif // USE_MODULE_VLCCORE_HTTP
}

// - HTTP plugin implementation -

#ifdef _MSC_VER
  #pragma warning (disable:4005)        // C4005: macro redefinition (ENETUNREACH)
#endif // _MSC_VER

#ifndef MODULE_STRING
  #define MODULE_STRING "main"  // needed by VLC
#endif // MODULE_STRING

extern "C" {
  #include <vlc_plugin.h>

  #include <vlc_access.h>

  #include <vlc_meta.h>
  #include <vlc_network.h>
  #include <vlc_url.h>
  #include <vlc_tls.h>
  #include <vlc_strings.h>
  #include <vlc_charset.h>
  #include <vlc_input.h>
  //#include <vlc_md5.h>
  #include <vlc_http.h>
  #include <vlc_configuration.h>

#ifdef HAVE_ZLIB_H
  #include <zlib.h>
#endif
#ifdef HAVE_LIBPROXY // jichi 2/21/2012: not used
  #include <proxy.h>
#endif
} // extern "C"

//#ifdef WIN32
//  #include <windows.h>
//
//  #define strcasecmp(_s1,_s2)           stricmp(_s1,_s2)
//  #define strncasecmp(_s1,_s2,_n)       strnicmp(_s1,_s2,_n)
//  #define strcasestr(_s1,_s2)           stristr(_s1,_s2)
//
//  #define stristr(_s1,_s2)              strstr(_s1,_s2) // FIXME: unimplemented, see: http://stackoverflow.com/questions/211535/fastest-way-to-do-a-case-insensitive-substring-search-in-c-c
//  #define strtoll(_nptr,_endptr,_base)  strtol(_nptr,_endptr,_base) // FIXME: lose precision
//  #define atoll(_str)                   atol(_str)      // FIXME: lose precision
//
//  #define PRIu64        "llu"
//  #define SCNu64        "llu"
//  #define PRId64        "ll"
//#endif // WIN32

struct access_sys_t
{ VlcHttpSession session; };

int
VlcHttpPlugin::open(vlc_object_t *p_this)
{
  Q_ASSERT(p_this);
  DOUT("enter");
  access_t *p_access = reinterpret_cast<access_t *>(p_this);

  //STANDARD_READ_ACCESS_INIT;
  ::access_InitFields(p_access);
  ACCESS_SET_CALLBACKS(read, NULL, control, seek); // read, block, control, seek

  access_sys_t *p_sys = p_access->p_sys = new access_sys_t;
  VlcHttpSession &session = p_sys->session;

  QString path = QString::fromLocal8Bit(p_access->psz_path).trimmed();
  QString protocal = QString::fromLocal8Bit(p_access->psz_access);
  QString url = protocal + "://" + path;
  session.setUrl(url);
  DOUT("url =" << url);

  //p_access->info.i_size = 0;
  //p_access->info.i_pos = 0;
  //p_access->info.b_eof = false;

  session.run();

  DOUT("eventloop enter");
  QEventLoop loop;
  QObject::connect(&session, SIGNAL(error(QString)), &loop, SLOT(quit()));
  QObject::connect(&session, SIGNAL(finished()), &loop, SLOT(quit()));
  QObject::connect(&session, SIGNAL(readyRead()), &loop, SLOT(quit()));
  QObject::connect(&session, SIGNAL(stopped()), &loop, SLOT(quit()));
  loop.exec();
  DOUT("eventloop leave");

//#ifdef HAVE_ZLIB_H
//  p_access->pf_read = ReadCompressed;
//
//  p_sys->b_compressed = false;
//  /* 15 is the max windowBits, +32 to enable optional gzip decoding */
//  if( inflateInit2( &p_sys->inflate.stream, 32+15 ) != Z_OK )
//    msg_Warn( p_access, "Error during zlib initialisation: %s",
//          p_sys->inflate.stream.msg );
//  if( zlibCompileFlags() & (1<<17) )
//    msg_Warn( p_access, "Your zlib was compiled without gzip support." );
//  p_sys->inflate.p_buffer = NULL;
//#endif

  //http_auth_Init( &p_sys->auth );
  //http_auth_Init( &p_sys->proxy_auth );

  //char *p, *psz;
  //p = psz = strdup( p_access->psz_path );
  //while( (p = strchr( p, ' ' )) != NULL )
  //  *p = '+';
  //vlc_UrlParse( &p_sys->url, psz, 0 );
  //free( psz );

  //if( p_sys->url.psz_host == NULL || *p_sys->url.psz_host == '\0' )
  //{
  //  msg_Warn( p_access, "invalid host" );
  //  DOUT("error: invalid host" );
  //  goto error;
  //}
  //if( !strncmp( psz_access, "https", 5 ) )
  //{
  //  /* HTTP over SSL */
  //  p_sys->b_ssl = true;
  //  if( p_sys->url.i_port <= 0 )
  //    p_sys->url.i_port = 443;
  //}
  //else
  //{
  //  if( p_sys->url.i_port <= 0 )
  //    p_sys->url.i_port = 80;
  //}

  // Determine the HTTP user agent
  // See RFC2616 §2.2 token definition and §3.8 user-agent header
  //p_sys->psz_user_agent = var_CreateGetString( p_access, "http-user-agent" );
  //for( char *p = p_sys->psz_user_agent; *p; p++ )
  //{
  //  uint8_t c = *p;
  //  if( c < 32 || strchr( "()<>@,;:\\\"[]?={}", c ) )
  //    *p = '_'; /* remove potentially harmful characters */
  //}


  //p_sys->b_reconnect = var_CreateGetBool( p_access, "http-reconnect" );
  //p_sys->b_continuous = var_CreateGetBool( p_access, "http-continuous" );

  //if( ( p_sys->i_code == 301 || p_sys->i_code == 302 ||
  //    p_sys->i_code == 303 || p_sys->i_code == 307 ) &&
  //  p_sys->psz_location && *p_sys->psz_location )
  //{
  //  msg_Dbg( p_access, "redirection to %s", p_sys->psz_location );
//
  //  /* Check the number of redirection already done */
  //  if( i_nb_redirect >= i_max_redirect )
  //  {
  //    msg_Err( p_access, "Too many redirection: break potential infinite"
  //         "loop" );
  //    goto error;
  //  }


  //if( p_sys->psz_mime &&
  //     !strncasecmp( p_sys->psz_mime, "application/xspf+xml", 20 ) &&
  //     ( memchr( " ;\t", p_sys->psz_mime[20], 4 ) != NULL ) )
  //{
  //  free( p_access->psz_demux );
  //  p_access->psz_demux = strdup( "xspf-open" );
  //}

  // PTS delay
  //var_Create( p_access, "http-caching", VLC_VAR_INTEGER |VLC_VAR_DOINHERIT );

  //bool ok = session.isRunning() || session.isFinished();
  bool ok = true;
  DOUT("exit: ret =" << ok);
  return ok ? VLC_SUCCESS : VLC_EGENERIC;
}

void
VlcHttpPlugin::close(vlc_object_t *p_this)
{
  Q_ASSERT(p_this);
  DOUT("enter");
  access_t *p_access = reinterpret_cast<access_t *>(p_this);
  access_sys_t *p_sys = p_access->p_sys;
  p_access->p_sys = 0;
  if (p_sys) {
    p_sys->session.stop();
    delete p_sys;
  }
  DOUT("exit");
}

// Read: Read up to i_len bytes from the http connection and place in
// p_buffer. Return the actual number of bytes read
ssize_t
VlcHttpPlugin::read(access_t *p_access, uint8_t *p_buffer, size_t i_len)
{
  DOUT("enter: i_len =" << i_len);
  Q_ASSERT(p_access && p_access->p_sys);
  VlcHttpSession &session = p_access->p_sys->session;

  qint64 count = session.read((char *)p_buffer, i_len);

  p_access->info.i_size = session.size();
  p_access->info.i_pos = session.pos();
  p_access->info.b_eof = count != (qint64)i_len;

  DOUT("exit: ret =" << count);
  return count;

  //if( p_sys->fd == -1 )
  //{
  //  p_access->info.b_eof = true;
  //  return 0;
  //}

  //if (p_sys->b_has_size) {
  //  // Remaining bytes in the file
  //  uint64_t remainder = p_access->info.i_size - p_access->info.i_pos;
  //  if( remainder < i_len )
  //    i_len = remainder;
//
  //  /* Remaining bytes in the response */
  //  if( p_sys->i_remaining < i_len )
  //    i_len = p_sys->i_remaining;
  //}


  //i_read = net_Read( p_access, p_sys->fd, p_sys->p_vs, p_buffer, i_len, false );
  //if( i_read > 0 )
  //{
  //  if( p_sys->b_chunked )
  //  {
  //    p_sys->i_chunk -= i_read;
  //    if( p_sys->i_chunk <= 0 )
  //    {
  //      /* read the empty line */
  //      char *psz = net_Gets( p_access, p_sys->fd, p_sys->p_vs );
  //      free( psz );
  //    }
  //  }
  //}
  //  if( i_read <= 0 )
  //  {
  //    p_access->info.b_eof = true;
  //    if( i_read < 0 )
  //      p_sys->b_error = true;
  //    return 0;
  //  }

  //assert( i_read >= 0 );
  //p_access->info.i_pos += i_read;
  //if( p_sys->b_has_size )
  //{
  //  assert( p_access->info.i_pos <= p_access->info.i_size );
  //  assert( (unsigned)i_read <= p_sys->i_remaining );
  //  p_sys->i_remaining -= i_read;
  //}
}

/*
#ifdef HAVE_ZLIB_H
ssize_t
VlcHttpPlugin::readCompressed( access_t *p_access, uint8_t *p_buffer, size_t i_len )
{
  access_sys_t *p_sys = p_access->p_sys;

  if( p_sys->b_compressed )
  {
    int i_ret;

    if( !p_sys->inflate.p_buffer )
      p_sys->inflate.p_buffer = (uint8_t *)malloc( 256 * 1024 );

    if( p_sys->inflate.stream.avail_in == 0 )
    {
      ssize_t i_read = Read( p_access, p_sys->inflate.p_buffer + p_sys->inflate.stream.avail_in, 256 * 1024 );
      if( i_read <= 0 ) return i_read;
      p_sys->inflate.stream.next_in = p_sys->inflate.p_buffer;
      p_sys->inflate.stream.avail_in = i_read;
    }

    p_sys->inflate.stream.avail_out = i_len;
    p_sys->inflate.stream.next_out = p_buffer;

    i_ret = inflate( &p_sys->inflate.stream, Z_SYNC_FLUSH );
    msg_Warn( p_access, "inflate return value: %d, %s", i_ret, p_sys->inflate.stream.msg );

    return i_len - p_sys->inflate.stream.avail_out;
  }
  else
  {
    return Read( p_access, p_buffer, i_len );
  }
}
#endif
*/

// Seek: close and re-open a connection at the right place
int
VlcHttpPlugin::seek(access_t *p_access, uint64_t i_pos)
{
  DOUT("enter: i_pos =" << i_pos);

  VlcHttpSession &session = p_access->p_sys->session;
  session.seek(i_pos);

  //p_access->info.i_size = session.size();
  //p_access->info.i_pos = session.pos();

  //Disconnect( p_access );

  //if (p_access->info.i_size &&
  //    i_pos >= p_access->info.i_size) {
  //  DOUT( "seek to far, seek to eof");
  //  return seek(p_access, p_access->info.i_size - 1);
  //}

  //if(Connect( p_access, i_pos))
  //{
  //  msg_Err( p_access, "seek failed" );
  //  p_access->info.b_eof = true;
  //  return VLC_EGENERIC;
  //}
  return VLC_SUCCESS;
}

int
VlcHttpPlugin::control(access_t *p_access, int i_query, va_list args)
{
  DOUT("enter: i_query =" << i_query);
  Q_ASSERT(session);
  //access_sys_t *p_sys = p_access->p_sys;
  bool     *pb_bool;
  int64_t  *pi_64;
  //vlc_meta_t *p_meta;

  switch( i_query ) {
  case ACCESS_CAN_SEEK:
    DOUT("query = ACCESS_CAN_SEEK");
    pb_bool = (bool*)va_arg( args, bool* );
    //*pb_bool = p_sys->b_seekable;
    *pb_bool = true; // FIXME: not seekable
    break;
  case ACCESS_CAN_FASTSEEK:
    DOUT("query = ACCESS_CAN_FASTSEEK");
    pb_bool = (bool*)va_arg( args, bool* );
    *pb_bool = false;
    break;
  case ACCESS_CAN_PAUSE:
  case ACCESS_CAN_CONTROL_PACE:
    DOUT("query = ACCESS_CAN_PAUSE");
    pb_bool = (bool*)va_arg( args, bool* );
    *pb_bool = true;
    break;

  case ACCESS_GET_PTS_DELAY:
    DOUT("query = ACCESS_GET_PTS_DELAY");
    pi_64 = (int64_t*)va_arg( args, int64_t * );
    //*pi_64 = (int64_t)var_GetInteger( p_access, "http-caching" ) * 1000;
    *pi_64 = 1000;
    break;

  case ACCESS_SET_PAUSE_STATE:
    DOUT("query = ACCESS_SET_PAUSE_STATE");
    break;

  case ACCESS_GET_CONTENT_TYPE: // FIXME: get content type
    {
      DOUT("query = ACCESS_GET_CONTENT_TYPE");
      Q_ASSERT(p_access->p_sys);
      VlcHttpSession &session = p_access->p_sys->session;
      QString t = session.contentType();
      if (t.isEmpty())
        *va_arg(args, char **) = NULL;
      else
        *va_arg(args, char **) = ::strdup(t.toLocal8Bit());
    } break;

  case ACCESS_GET_TITLE_INFO:
  case ACCESS_SET_TITLE:
  case ACCESS_SET_SEEKPOINT:
  case ACCESS_SET_PRIVATE_ID_STATE:
    DOUT("exit: ignored");
    return VLC_EGENERIC;

  case ACCESS_GET_META:
    DOUT("query = ACCESS_GET_META");
    //p_meta = (vlc_meta_t*)va_arg( args, vlc_meta_t* );
    //if( p_sys->psz_icy_name )
    //  vlc_meta_Set( p_meta, vlc_meta_Title, p_sys->psz_icy_name );
    //if( p_sys->psz_icy_genre )
    //  vlc_meta_Set( p_meta, vlc_meta_Genre, p_sys->psz_icy_genre );
    //if( p_sys->psz_icy_title )
    //  vlc_meta_Set( p_meta, vlc_meta_NowPlaying, p_sys->psz_icy_title );
    break;

  default:
    DOUT("exit: ERROR: unimplemented query in control");
    return VLC_EGENERIC;
  }
  DOUT("exit: success");
  return VLC_SUCCESS;
}

// EOF

/*

static int Connect( access_t *p_access, uint64_t i_tell )
{
  DOUT("enter");
  access_sys_t   *p_sys = p_access->p_sys;
  vlc_url_t    srv = p_sys->b_proxy ? p_sys->proxy : p_sys->url;

  // Clean info
  free( p_sys->psz_location );
  free( p_sys->psz_mime );
  free( p_sys->psz_pragma );

  free( p_sys->psz_icy_genre );
  free( p_sys->psz_icy_name );
  free( p_sys->psz_icy_title );


  p_sys->psz_location = NULL;
  p_sys->psz_mime = NULL;
  p_sys->psz_pragma = NULL;
  p_sys->b_mms = false;
  p_sys->b_chunked = false;
  p_sys->i_chunk = 0;
  p_sys->i_icy_meta = 0;
  p_sys->i_icy_offset = i_tell;
  p_sys->psz_icy_name = NULL;
  p_sys->psz_icy_genre = NULL;
  p_sys->psz_icy_title = NULL;
  p_sys->i_remaining = 0;
  p_sys->b_persist = false;
  p_sys->b_has_size = false;
  p_access->info.i_size = 0;
  p_access->info.i_pos  = i_tell;
  p_access->info.b_eof  = false;

  // Open connection
  assert( p_sys->fd == -1 ); // No open sockets (leaking fds is BAD)
  p_sys->fd = net_ConnectTCP( p_access, srv.psz_host, srv.i_port );
  if( p_sys->fd == -1 )
  {
    msg_Err( p_access, "cannot connect to %s:%d", srv.psz_host, srv.i_port );
    DOUT(QString("exit: cannot connect to %1:%2").arg(QString::fromLocal8Bit(srv.psz_host)).arg(QString::number(srv.i_port)));
    return -1;
  }
  int optval = 1;
  //setsockopt (p_sys->fd, SOL_SOCKET, SO_KEEPALIVE, &(int){ 1 }, sizeof (int));
  setsockopt (p_sys->fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof (int));

  // Initialize TLS/SSL session
  if( p_sys->b_ssl == true )
  {
    // CONNECT to establish TLS tunnel through HTTP proxy
    if( p_sys->b_proxy )
    {
      char *psz;
      unsigned i_status = 0;

      if( p_sys->i_version == 0 )
      {
        // CONNECT is not in HTTP/1.0
        Disconnect( p_access );
        DOUT("exit: error: connect is not in HTTP/1.0");
        return -1;
      }

      net_Printf( p_access, p_sys->fd, NULL,
            "CONNECT %s:%d HTTP/1.%d\r\nHost: %s:%d\r\n\r\n",
            p_sys->url.psz_host, p_sys->url.i_port,
            p_sys->i_version,
            p_sys->url.psz_host, p_sys->url.i_port);

      psz = net_Gets( p_access, p_sys->fd, NULL );
      if( psz == NULL )
      {
        msg_Err( p_access, "cannot establish HTTP/TLS tunnel" );
        Disconnect( p_access );
        DOUT("exit: error: cannot establish HTTP/TLS tunnel");
        return -1;
      }

      sscanf( psz, "HTTP/%*u.%*u %3u", &i_status );
      free( psz );

      if( ( i_status / 100 ) != 2 )
      {
        msg_Err( p_access, "HTTP/TLS tunnel through proxy denied" );
        Disconnect( p_access );
        DOUT("exit: error: HTTP/TLS tunnel through proxy denied");
        return -1;
      }

      do
      {
        psz = net_Gets( p_access, p_sys->fd, NULL );
        if( psz == NULL )
        {
          msg_Err( p_access, "HTTP proxy connection failed" );
          Disconnect( p_access );
          DOUT("exit: error: HTTP proxy connection failed");
          return -1;
        }

        if( *psz == '\0' )
          i_status = 0;

        free( psz );

        if( !vlc_object_alive (p_access) || p_sys->b_error )
        {
          Disconnect( p_access );
          return -1;
        }
      }
      while( i_status );
    }

    // TLS/SSL handshake
    p_sys->p_tls = tls_ClientCreate( VLC_OBJECT(p_access), p_sys->fd,
                     p_sys->url.psz_host );
    if( p_sys->p_tls == NULL )
    {
      msg_Err( p_access, "cannot establish HTTP/TLS session" );
      Disconnect( p_access );
      DOUT("exit: error: cannot establish HTTP/TLS session");
      return -1;
    }
    p_sys->p_vs = &p_sys->p_tls->sock;
  }

  int ret = Request( p_access, i_tell ) ? -2 : 0;
  DOUT("exit: ret =" << ret);
  return ret;
}

static int Request( access_t *p_access, uint64_t i_tell )
{
  DOUT("enter: i_tell =" << i_tell);
  access_sys_t   *p_sys = p_access->p_sys;
  char       *psz ;
  v_socket_t   *pvs = p_sys->p_vs;
  p_sys->b_persist = false;

  p_sys->i_remaining = 0;
  if( p_sys->b_proxy )
  {
    DOUT("using proxy");
    if( p_sys->url.psz_path )
    {
      net_Printf( p_access, p_sys->fd, NULL,
            "GET http://%s:%d%s HTTP/1.%d\r\n",
            p_sys->url.psz_host, p_sys->url.i_port,
            p_sys->url.psz_path, p_sys->i_version );
    }
    else
    {
      net_Printf( p_access, p_sys->fd, NULL,
            "GET http://%s:%d/ HTTP/1.%d\r\n",
            p_sys->url.psz_host, p_sys->url.i_port,
            p_sys->i_version );
    }
  }
  else
  {
    const char *psz_path = p_sys->url.psz_path;
    if( !psz_path || !*psz_path )
    {
      psz_path = "/";
    }
    if( p_sys->url.i_port != (pvs ? 443 : 80) )
    {
      net_Printf( p_access, p_sys->fd, pvs,
            "GET %s HTTP/1.%d\r\nHost: %s:%d\r\n",
            psz_path, p_sys->i_version, p_sys->url.psz_host,
            p_sys->url.i_port );
    }
    else
    {
      net_Printf( p_access, p_sys->fd, pvs,
            "GET %s HTTP/1.%d\r\nHost: %s\r\n",
            psz_path, p_sys->i_version, p_sys->url.psz_host );
    }
  }
  // User Agent
  DOUT("User-Agent =" << QString::fromLocal8Bit(p_sys->psz_user_agent));
  net_Printf( p_access, p_sys->fd, pvs,
        "User-Agent: %s\r\n",
        p_sys->psz_user_agent );
  // Offset
  if( p_sys->i_version == 1 && ! p_sys->b_continuous )
  {
    DOUT("set request offset");
    p_sys->b_persist = true;
    net_Printf( p_access, p_sys->fd, pvs,
          "Range: bytes=%"PRIu64"-\r\n", i_tell );
  }

  // Cookies
  if( p_sys->cookies )
  {
    DOUT("set request cookies");
    int i;
    for( i = 0; i < vlc_array_count( p_sys->cookies ); i++ )
    {
      const char  *cookie = (char *)vlc_array_item_at_index( p_sys->cookies, i );
      char * psz_cookie_content = cookie_get_content( cookie );
      char * psz_cookie_domain = cookie_get_domain( cookie );

      assert( psz_cookie_content );

      // FIXME: This is clearly not conforming to the rfc
      bool is_in_right_domain = (!psz_cookie_domain || strstr( p_sys->url.psz_host, psz_cookie_domain ));

      if( is_in_right_domain)
      {
        msg_Dbg( p_access, "Sending Cookie %s", psz_cookie_content );
        DOUT("sending cookie:" << QString::fromLocal8Bit(psz_cookie_content));
        if( net_Printf( p_access, p_sys->fd, pvs, "Cookie: %s\r\n", psz_cookie_content ) < 0 ) {
          msg_Err( p_access, "failed to send cookie" );
          DOUT("warning: failed to send cookie");
        }
      }
      free( psz_cookie_content );
      free( psz_cookie_domain );
    }
  }

  // Authentication
  if( p_sys->url.psz_username || p_sys->url.psz_password ) {
    DOUT("authentification request");
    AuthReply( p_access, "", &p_sys->url, &p_sys->auth );
  }

  // Proxy Authentication
  if( p_sys->proxy.psz_username || p_sys->proxy.psz_password ) {
    DOUT("proxy authentification request");
    AuthReply( p_access, "Proxy-", &p_sys->proxy, &p_sys->proxy_auth );
  }

  if( net_Printf( p_access, p_sys->fd, pvs, "\r\n" ) < 0 )
  {
    msg_Err( p_access, "failed to send request" );
    Disconnect( p_access );
    DOUT("exit: ret = egeneric, failed to send request");
    return VLC_EGENERIC;
  }

  // Read Answer
  if( ( psz = net_Gets( p_access, p_sys->fd, pvs ) ) == NULL )
  {
    msg_Err( p_access, "failed to read answer" );
    DOUT("error, failed to read answer");
    goto error;
  }
  if( !strncmp( psz, "HTTP/1.", 7 ) )
  {
    p_sys->psz_protocol = "HTTP";
    p_sys->i_code = atoi( &psz[9] );
  }
  else if( !strncmp( psz, "ICY", 3 ) )
  {
    p_sys->psz_protocol = "ICY";
    p_sys->i_code = atoi( &psz[4] );
    p_sys->b_reconnect = true;
  }
  else
  {
    msg_Err( p_access, "invalid HTTP reply '%s'", psz );
    free( psz );
    DOUT("error, invalid HTTP reply:" << QString::fromLocal8Bit(psz));
    goto error;
  }
  msg_Dbg( p_access, "protocol '%s' answer code %d",
       p_sys->psz_protocol, p_sys->i_code );
  if( !strcmp( p_sys->psz_protocol, "ICY" ) )
  {
    p_sys->b_seekable = false;
  }
  if( p_sys->i_code != 206 && p_sys->i_code != 401 )
  {
    p_sys->b_seekable = false;
  }
  // Authentication error - We'll have to display the dialog
  if( p_sys->i_code == 401 )
  {
    DOUT("http error 401: authentification required");
  }
  // Other fatal error
  else if( p_sys->i_code >= 400 )
  {
    msg_Err( p_access, "error: %s", psz );
    free( psz );
    DOUT("got error message:" << QString::fromLocal8Bit(psz));
    goto error;
  }
  free( psz );

  for( ;; )
  {
    char *psz = net_Gets( p_access, p_sys->fd, pvs );
    char *p;

    if( psz == NULL )
    {
      msg_Err( p_access, "failed to read answer" );
      DOUT("error: failed to read answer");
      goto error;
    }

    if( !vlc_object_alive (p_access) || p_sys->b_error )
    {
      free( psz );
      DOUT("error: access is not alived");
      goto error;
    }

    // msg_Dbg( p_input, "Line=%s", psz );
    if( *psz == '\0' )
    {
      free( psz );
      break;
    }

    if( ( p = strchr( psz, ':' ) ) == NULL )
    {
      msg_Err( p_access, "malformed header line: %s", psz );
      free( psz );
      DOUT("error: malformed header line:" << QString::fromLocal8Bit(psz));
      goto error;
    }
    *p++ = '\0';
    while( *p == ' ' ) p++;

    if( !strcasecmp( psz, "Content-Length" ) )
    {
      uint64_t i_size = i_tell + (p_sys->i_remaining = (uint64_t)atoll( p ));
      if(i_size > p_access->info.i_size) {
        p_sys->b_has_size = true;
        p_access->info.i_size = i_size;
      }
      msg_Dbg( p_access, "this frame size=%"PRIu64, p_sys->i_remaining );
      DOUT("Content-Length =" << p_sys->i_remaining);
    }
    else if( !strcasecmp( psz, "Content-Range" ) ) {
      uint64_t i_ntell = i_tell;
      uint64_t i_nend = (p_access->info.i_size > 0)?(p_access->info.i_size - 1):i_tell;
      uint64_t i_nsize = p_access->info.i_size;
      sscanf(p,"bytes %"SCNu64"-%"SCNu64"/%"SCNu64,&i_ntell,&i_nend,&i_nsize);
      if(i_nend > i_ntell ) {
        p_access->info.i_pos = i_ntell;
        p_sys->i_icy_offset  = i_ntell;
        p_sys->i_remaining = i_nend+1-i_ntell;
        int64_t i_size = (i_nsize > i_nend) ? i_nsize : (i_nend + 1);
        if(i_size > (int64_t)p_access->info.i_size) {
          p_sys->b_has_size = true;
          p_access->info.i_size = i_size;
        }
        msg_Dbg( p_access, "stream size=%"PRIu64",pos=%"PRIu64",remaining=%"PRIu64,
             i_nsize, i_ntell, p_sys->i_remaining);
      DOUT("got Content-Range");
      }
    }
    else if( !strcasecmp( psz, "Connection" ) ) {
      msg_Dbg( p_access, "Connection: %s",p );
      int i = -1;
      sscanf(p, "close%n",&i);
      if( i >= 0 ) {
        p_sys->b_persist = false;
      }
      DOUT("Connection =" << QString::fromLocal8Bit(p));
    }
    else if( !strcasecmp( psz, "Location" ) )
    {
      DOUT("got Location");
      char * psz_new_loc;

      // This does not follow RFC 2068, but yet if the url is not absolute, handle it as everyone does.
      if( p[0] == '/' )
      {
        const char *psz_http_ext = p_sys->b_ssl ? "s" : "" ;

        if( p_sys->url.i_port == ( p_sys->b_ssl ? 443 : 80 ) )
        {
          if( asprintf(&psz_new_loc, "http%s://%s%s", psz_http_ext,
                 p_sys->url.psz_host, p) < 0 )
            DOUT("error: http_ext =" << QString::fromLocal8Bit(psz_http_ext));
            goto error;
        }
        else
        {
          if( asprintf(&psz_new_loc, "http%s://%s:%d%s", psz_http_ext,
                 p_sys->url.psz_host, p_sys->url.i_port, p) < 0 )
            DOUT("error: http_ext =" << QString::fromLocal8Bit(psz_http_ext));
            goto error;
        }
      }
      else
      {
        psz_new_loc = strdup( p );
      }

      free( p_sys->psz_location );
      p_sys->psz_location = psz_new_loc;
    }
    else if( !strcasecmp( psz, "Content-Type" ) )
    {
      free( p_sys->psz_mime );
      p_sys->psz_mime = strdup( p );
      msg_Dbg( p_access, "Content-Type: %s", p_sys->psz_mime );
    }
    else if( !strcasecmp( psz, "Content-Encoding" ) )
    {
      DOUT("Content-Encoding =" << QString::fromLocal8Bit(p));
      msg_Dbg( p_access, "Content-Encoding: %s", p );
      if( !strcasecmp( p, "identity" ) )
        ;
#ifdef HAVE_ZLIB_H
      else if( !strcasecmp( p, "gzip" ) || !strcasecmp( p, "deflate" ) )
        p_sys->b_compressed = true;
#endif
      else
        msg_Warn( p_access, "Unknown content coding: %s", p );
      DOUT("warn: unknown content encoding");
    }
    else if( !strcasecmp( psz, "Pragma" ) )
    {
      DOUT("got pragma");
      if( !strcasecmp( psz, "Pragma: features" ) )
        p_sys->b_mms = true;
      free( p_sys->psz_pragma );
      p_sys->psz_pragma = strdup( p );
      msg_Dbg( p_access, "Pragma: %s", p_sys->psz_pragma );
    }
    else if( !strcasecmp( psz, "Server" ) )
    {
      DOUT("Server =" << QString::fromLocal8Bit(p));
      msg_Dbg( p_access, "Server: %s", p );
      if( !strncasecmp( p, "Icecast", 7 ) ||
        !strncasecmp( p, "Nanocaster", 10 ) )
      {
        p_sys->b_reconnect = true;
        p_sys->b_pace_control = false;
        p_sys->b_icecast = true;
      }
    }
    else if( !strcasecmp( psz, "Transfer-Encoding" ) )
    {
      DOUT("Transfer-Encoding =" << QString::fromLocal8Bit(p));
      msg_Dbg( p_access, "Transfer-Encoding: %s", p );
      if( !strncasecmp( p, "chunked", 7 ) )
      {
        p_sys->b_chunked = true;
      }
    }
    else if( !strcasecmp( psz, "Icy-MetaInt" ) )
    {
      DOUT("Icy-MetaInt =" << QString::fromLocal8Bit(p));
      msg_Dbg( p_access, "Icy-MetaInt: %s", p );
      p_sys->i_icy_meta = atoi( p );
      if( p_sys->i_icy_meta < 0 )
        p_sys->i_icy_meta = 0;
      if( p_sys->i_icy_meta > 0 )
        p_sys->b_icecast = true;

      msg_Warn( p_access, "ICY metaint=%d", p_sys->i_icy_meta );
    }
    else if( !strcasecmp( psz, "Icy-Name" ) )
    {
      DOUT("got Icy-Name");
      free( p_sys->psz_icy_name );
      char *psz_tmp = strdup( p );
      p_sys->psz_icy_name = EnsureUTF8( psz_tmp );
      if( !p_sys->psz_icy_name )
        free( psz_tmp );
      msg_Dbg( p_access, "Icy-Name: %s", p_sys->psz_icy_name );

      p_sys->b_icecast = true; // be on the safeside. set it here as well.
      p_sys->b_reconnect = true;
      p_sys->b_pace_control = false;
    }
    else if( !strcasecmp( psz, "Icy-Genre" ) )
    {
      DOUT("got Icy-Genre");
      free( p_sys->psz_icy_genre );
      char *psz_tmp = strdup( p );
      p_sys->psz_icy_genre = EnsureUTF8( psz_tmp );
      if( !p_sys->psz_icy_genre )
        free( psz_tmp );
      msg_Dbg( p_access, "Icy-Genre: %s", p_sys->psz_icy_genre );
    }
    else if( !strncasecmp( psz, "Icy-Notice", 10 ) )
    {
      DOUT("got Icy-Notice");
      msg_Dbg( p_access, "Icy-Notice: %s", p );
    }
    else if( !strncasecmp( psz, "icy-", 4 ) ||
         !strncasecmp( psz, "ice-", 4 ) ||
         !strncasecmp( psz, "x-audiocast", 11 ) )
    {
      DOUT("got Meta-Info");
      msg_Dbg( p_access, "Meta-Info: %s: %s", psz, p );
    }
    else if( !strcasecmp( psz, "Set-Cookie" ) )
    {
      DOUT("Set-Cookie");
      if( p_sys->cookies )
      {
        msg_Dbg( p_access, "Accepting Cookie: %s", p );
        cookie_append( p_sys->cookies, strdup(p) );
      }
      else {
        DOUT("warn: cookie ignored");
        msg_Dbg( p_access, "We have a Cookie we won't remember: %s", p );
      }
    }
    else if( !strcasecmp( psz, "www-authenticate" ) )
    {
      DOUT("www-authenticate");
      msg_Dbg( p_access, "Authentication header: %s", p );
      http_auth_ParseWwwAuthenticateHeader( VLC_OBJECT(p_access),
                          &p_sys->auth, p );
    }
    else if( !strcasecmp( psz, "proxy-authenticate" ) )
    {
      DOUT("proxy-authenticate");
      msg_Dbg( p_access, "Proxy authentication header: %s", p );
      http_auth_ParseWwwAuthenticateHeader( VLC_OBJECT(p_access),
                          &p_sys->proxy_auth, p );
    }
    else if( !strcasecmp( psz, "authentication-info" ) )
    {
      DOUT("authentication-info");
      msg_Dbg( p_access, "Authentication Info header: %s", p );
      if( AuthCheckReply( p_access, p, &p_sys->url, &p_sys->auth ) )
        goto error;
    }
    else if( !strcasecmp( psz, "proxy-authentication-info" ) )
    {
      DOUT("proxy-authentication-info");
      msg_Dbg( p_access, "Proxy Authentication Info header: %s", p );
      if( AuthCheckReply( p_access, p, &p_sys->proxy, &p_sys->proxy_auth ) )
        goto error;
    }

    free( psz );
  }
  // We close the stream for zero length data, unless of course the
  // server has already promised to do this for us.
  if( p_sys->b_has_size && p_sys->i_remaining == 0 && p_sys->b_persist ) {
    Disconnect( p_access );
  }
  DOUT("exit: ret = success");
  return VLC_SUCCESS;

error:
  Disconnect( p_access );
  DOUT("exit: ret = egeneric");
  return VLC_EGENERIC;
}

static void Disconnect( access_t *p_access )
{
  DOUT("enter");
  access_sys_t *p_sys = p_access->p_sys;

  if( p_sys->p_tls != NULL)
  {
    tls_ClientDelete( p_sys->p_tls );
    p_sys->p_tls = NULL;
    p_sys->p_vs = NULL;
  }
  if( p_sys->fd != -1)
  {
    net_Close(p_sys->fd);
    p_sys->fd = -1;
  }
  DOUT("exit");
}

vlc_module_begin ()
  set_description( N_("HTTP input") )
  set_capability( "access", 0 )
  set_shortname( N_( "HTTP(S)" ) )
  set_category( CAT_INPUT )
  set_subcategory( SUBCAT_INPUT_ACCESS )

  add_string( "http-proxy", NULL, NULL, PROXY_TEXT, PROXY_LONGTEXT,
        false )
  add_password( "http-proxy-pwd", NULL, NULL,
          PROXY_PASS_TEXT, PROXY_PASS_LONGTEXT, false )
  add_integer( "http-caching", 4 * DEFAULT_PTS_DELAY / 1000, NULL,
         CACHING_TEXT, CACHING_LONGTEXT, true )
    change_safe()
  add_string( "http-user-agent", PACKAGE_NAME"/"PACKAGE_VERSION, NULL,
        AGENT_TEXT, AGENT_LONGTEXT, true )
    change_safe()
    change_need_restart()
  add_bool( "http-reconnect", false, NULL, RECONNECT_TEXT,
        RECONNECT_LONGTEXT, true )
  add_bool( "http-continuous", false, NULL, CONTINUOUS_TEXT,
        CONTINUOUS_LONGTEXT, true )
    change_safe()
  add_bool( "http-forward-cookies", true, NULL, FORWARD_COOKIES_TEXT,
        FORWARD_COOKIES_LONGTEXT, true )
  add_integer( "http-max-redirect", 5, NULL, MAX_REDIRECT_TEXT,
         MAX_REDIRECT_LONGTEXT, true )
#ifdef WIN32
  add_bool( "http-use-IE-proxy", false, NULL, USE_IE_PROXY_TEXT,
        USE_IE_PROXY_LONGTEXT, true )
#endif
  add_obsolete_string("http-user")
  add_obsolete_string("http-pwd")
  add_shortcut( "http" )
  add_shortcut( "https" )
  add_shortcut( "unsv" )
  add_shortcut( "itpc" ) // iTunes Podcast
  add_shortcut( "icyx" )
  set_callbacks( Open, Close )
vlc_module_end ()

*/
