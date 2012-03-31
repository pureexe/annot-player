// vlccore/http.cc
// See: vlc/modules/access/http.c
// 2/21/2012
// Icy, Proxy, and Authentification supports are not implemented.

#include "module/vlccore/http.h"
#include "module/vlccore/httpsession.h"
#include "module/vlccore/httpbufferedsession.h"
#include "module/vlccore/httpstreamsession.h"
#include <QtCore>
#include <QtNetwork>
#include <cstdarg>

#include <vlc/plugins/vlc_common.h>
extern "C" {
  #include <vlc/src/modules/modules.h>
  module_t *module_find(const char *name);
} // extern "C"

#define DEBUG "vlccore::http"
#include "module/debug/debug.h"

// - Construction -

VlcHttpSession *VlcHttpPlugin::session_ = 0;
QString VlcHttpPlugin::url_;
QStringList VlcHttpPlugin::urls_;
qint64 VlcHttpPlugin::duration_ = 0;
QNetworkCookieJar *VlcHttpPlugin::cookieJar_ = 0;
QString VlcHttpPlugin::mediaTitle_;
bool VlcHttpPlugin::bufferSaved_ = true;

void
VlcHttpPlugin::setBufferSaved(bool t)
{
  bufferSaved_ = t;
  if (session_)
    session_->setBufferSaved(t);
}

bool
VlcHttpPlugin::isFinished()
{ return session_ && session_->isFinished(); }


void
VlcHttpPlugin::save()
{ if (session_) session_->save(); }

// - Static -

void
VlcHttpPlugin::load()
{
  DOUT("enter");
  module_t *m = ::module_find("access_http");
  Q_ASSERT(m);
  if (m) {
    DOUT("name =" << m->psz_shortname);
    DOUT("loaded =" << m->b_loaded << ", unloadable =" << m->b_unloadable);
    m->b_loaded = true;
    m->b_unloadable = false;
    m->pf_activate = (void *)open;
    m->pf_deactivate = (void *)close;
  }
  DOUT("exit");
}

void
VlcHttpPlugin::unload()
{
  DOUT("enter");
  module_t *m = ::module_find("access_http");
  Q_ASSERT(m);
  if (m) {
    m->pf_activate = 0;
    m->pf_deactivate = 0;
    m->b_loaded = false;
    m->b_unloadable = true;
  }
  DOUT("exit");
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

  //#include <vlc_meta.h>
  //#include <vlc_network.h>
  //#include <vlc_url.h>
  //#include <vlc_tls.h>
  //#include <vlc_strings.h>
  //#include <vlc_charset.h>
  //#include <vlc_input.h>
  //#include <vlc_md5.h>
  //#include <vlc_http.h>
  //#include <vlc_configuration.h>

//#ifdef HAVE_ZLIB_H
//  #include <zlib.h>
//#endif
//#ifdef HAVE_LIBPROXY
//  #include <proxy.h>
//#endif
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

//struct access_sys_t
//{ VlcHttpSession session; };

int
VlcHttpPlugin::open(vlc_object_t *p_this)
{
  Q_ASSERT(p_this);
  DOUT("enter");
  access_t *p_access = (access_t *)p_this;

  QString path = QString::fromLocal8Bit(p_access->psz_location).trimmed();
  QString protocal = QString::fromLocal8Bit(p_access->psz_access);
  QString url = protocal + "://" + path;

  DOUT("url =" << url);
  if (url.contains(".youtube.com/")) {
    // FIXME: youtube video not saved
    closeSession();
    DOUT("exit: youtube URL:" << url);
    return VLC_EGENERIC;
  }

  //STANDARD_READ_ACCESS_INIT;
  ::access_InitFields(p_access);
  ACCESS_SET_CALLBACKS(read, NULL, control, seek); // read, block, control, seek

  bool reuseLastSession = session_ && url_ == url;
  DOUT("reuseLastSession =" << reuseLastSession);
  if (reuseLastSession)
    session_->reset();
  else {
    closeSession();
    url_ = url;
    openSession();
  }

  Q_ASSERT(session_);

  bool ok = reuseLastSession || session_->isRunning();
  if (ok) {
    p_access->info.i_size = session_->size();
    p_access->info.i_pos = 0;
    p_access->info.b_eof = false;
  } else
    closeSession();
  DOUT("exit: ret =" << ok);
  return ok ? VLC_SUCCESS : VLC_EGENERIC;
}

void
VlcHttpPlugin::close(vlc_object_t *p_this)
{
  DOUT("enter");
  Q_UNUSED(p_this);
  //closeSession();
  DOUT("exit");
}

// - Session manager -

void
VlcHttpPlugin::openSession()
{
  DOUT("enter: session_ =" << session_);
  Q_ASSERT(!session_);

  if (!urls_.isEmpty()) {
    QList<QUrl> urls;
    foreach (QString u, urls_)
      urls.append(u);
    session_ = new HttpStreamSession(urls, duration_);
  } else
    session_ = new HttpBufferedSession(url_);

  session_->setBufferSaved(bufferSaved_);

  connect(session_, SIGNAL(error(QString)), globalInstance(), SIGNAL(error(QString)));
  connect(session_, SIGNAL(message(QString)), globalInstance(), SIGNAL(message(QString)));
  connect(session_, SIGNAL(fileSaved(QString)), globalInstance(), SIGNAL(fileSaved(QString)));
  connect(session_, SIGNAL(progress(qint64,qint64)), globalInstance(), SIGNAL(progress(qint64,qint64)));
  session_->setCookieJar(cookieJar_);
  session_->setMediaTitle(mediaTitle_);

  if (cookieJar_)
    cookieJar_->setParent(0); // memory leak

  session_->start();
  session_->waitForReady();

  DOUT("exit");
}

void
VlcHttpPlugin::closeSession()
{
  if (!session_)
    return;

  DOUT("enter");
  if (session_->isRunning()) {
    session_->stop();
    session_->waitForStopped();
    session_->wait(5000); // wait at most 5 seconds
  }
  session_->deleteLater();
  session_ = 0;
  DOUT("exit");
}

qint64
VlcHttpPlugin::duration()
{ return session_ ? session_->duration() : duration_; }

qint64
VlcHttpPlugin::availableDuration()
{ return session_ ? session_->availableDuration() : 0; }

qint64
VlcHttpPlugin::size()
{ return session_ ? session_->size() : 0; }

qint64
VlcHttpPlugin::availableSize()
{ return session_ ? session_->availableSize() : 0; }

// - I/O -

// Read: Read up to i_len bytes from the http connection and place in
// p_buffer. Return the actual number of bytes read
ssize_t
VlcHttpPlugin::read(access_t *p_access, uint8_t *p_buffer, size_t i_len)
{
  //DOUT("enter: i_len =" << i_len);
  //Q_ASSERT(p_access && p_access->p_sys);
  //VlcHttpSession *session = (VlcHttpSession *)p_access->p_sys;
  Q_ASSERT(p_access);
  Q_ASSERT(session_);

  qint64 count = session_->read((char *)p_buffer, i_len);

  p_access->info.i_pos = session_->pos();
  p_access->info.b_eof = count != (qint64)i_len;

  if (p_access->info.b_eof) {
    DOUT("read eof, sesion size =" << session_->size() << ", prevous size =" << p_access->info.i_size);
    p_access->info.i_size = session_->size();
  }

  //DOUT("exit: ret =" << count);
  return count;
}

// Seek: close and re-open a connection at the right place
int
VlcHttpPlugin::seek(access_t *p_access, uint64_t i_pos)
{
  //DOUT("enter: i_pos =" << i_pos);

  //VlcHttpSession *session = (VlcHttpSession *)p_access->p_sys;
  Q_ASSERT(session_);
  if (i_pos == (quint64)session_->pos()) {
    DOUT("exit: already seeked");
    return true;
  }
  //DOUT("size =" << p_access->info.i_size);
  //if (i_pos >= p_access->info.i_size) {
  //  DOUT("exit: ret = egeneric, out of size");
  //  return VLC_EGENERIC;
  //}

  if (i_pos >= (quint64)session_->availableSize()) {
    DOUT("egeneric, seek over availableSize: i_pos =" << i_pos << ", availableSize =" << session_->availableSize());
    //DOUT("exit: ret = egeneric, seek out of available size");
    return VLC_EGENERIC;
  }

  bool ok = session_->seek(i_pos);
  p_access->info.i_pos = i_pos;

  //DOUT("exit: ret =" << ok);
  return ok ? VLC_SUCCESS : VLC_EGENERIC;
}

int
VlcHttpPlugin::control(access_t *p_access, int i_query, va_list args)
{
  //DOUT("enter: i_query =" << i_query);
  Q_UNUSED(p_access);
  //access_sys_t *p_sys = p_access->p_sys;
  bool     *pb_bool;
  int64_t  *pi_64;
  //vlc_meta_t *p_meta;

  switch( i_query ) {
  case ACCESS_CAN_SEEK:
    //DOUT("query = ACCESS_CAN_SEEK");
    pb_bool = (bool *)va_arg(args, bool *);
    //*pb_bool = p_sys->b_seekable;
    *pb_bool = true; // FIXME: not seekable
    break;
  case ACCESS_CAN_FASTSEEK:
    //DOUT("query = ACCESS_CAN_FASTSEEK");
    pb_bool = (bool *)va_arg(args, bool *);
    *pb_bool = true;
    break;
  case ACCESS_CAN_PAUSE:
    //DOUT("query = ACCESS_CAN_PAUSE");
    pb_bool = (bool *)va_arg(args, bool*);
    *pb_bool = true;
    break;
  case ACCESS_CAN_CONTROL_PACE:
    //DOUT("query = ACCESS_CAN_CONTROL_PACE");
    pb_bool = (bool *)va_arg(args, bool *);
    *pb_bool = true;
    break;

  case ACCESS_GET_PTS_DELAY:
    //DOUT("query = ACCESS_GET_PTS_DELAY");
    pi_64 = (int64_t*)va_arg( args, int64_t * );
    //*pi_64 = (int64_t)var_GetInteger( p_access, "http-caching" ) * 1000;
    *pi_64 = 1000 * 1000; // in msecs, large enough to prevent jitter from youku video
    break;

  case ACCESS_SET_PAUSE_STATE:
    //DOUT("query = ACCESS_SET_PAUSE_STATE");
    break;

  case ACCESS_GET_CONTENT_TYPE: // FIXME: get content type
    {
      //DOUT("query = ACCESS_GET_CONTENT_TYPE");
      //Q_ASSERT(p_access->p_sys);
      //VlcHttpSession *session = (VlcHttpSession *)p_access->p_sys;
      Q_ASSERT(session_);
      QString t = session_->contentType();
      if (t.isEmpty())
        *va_arg(args, char **) = NULL;
      else
        *va_arg(args, char **) = ::strdup(t.toLocal8Bit());
    } break;

  case ACCESS_GET_TITLE_INFO:   // 0x102 = 258
  case ACCESS_SET_TITLE:
  case ACCESS_SET_SEEKPOINT:
  case ACCESS_SET_PRIVATE_ID_STATE:
    DOUT("exit: ignored: i_query =" << i_query);
    return VLC_EGENERIC;

  case ACCESS_GET_META:
    //DOUT("query = ACCESS_GET_META");
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
  //DOUT("exit: success");
  return VLC_SUCCESS;
}

// EOF

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

vlc_module_begin ()
    set_description( N_("HTTP input") )
    set_capability( "access", 0 )
    set_shortname( N_( "HTTP(S)" ) )
    set_category( CAT_INPUT )
    set_subcategory( SUBCAT_INPUT_ACCESS )

    add_string( "http-proxy", NULL, PROXY_TEXT, PROXY_LONGTEXT,
                false )
    add_password( "http-proxy-pwd", NULL,
                  PROXY_PASS_TEXT, PROXY_PASS_LONGTEXT, false )
    add_string( "http-referrer", NULL, REFERER_TEXT, REFERER_LONGTEXT, false )
        change_safe()
    add_string( "http-user-agent", NULL, UA_TEXT, UA_LONGTEXT, false )
        change_safe()
        change_private()
    add_bool( "http-reconnect", false, RECONNECT_TEXT,
              RECONNECT_LONGTEXT, true )
    add_bool( "http-continuous", false, CONTINUOUS_TEXT,
              CONTINUOUS_LONGTEXT, true )
        change_safe()
    add_bool( "http-forward-cookies", true, FORWARD_COOKIES_TEXT,
              FORWARD_COOKIES_LONGTEXT, true )
    // 'itpc' = iTunes Podcast
    add_shortcut( "http", "https", "unsv", "itpc", "icyx" )
    set_callbacks( Open, Close )
vlc_module_end ()
*/
