// vlccore/http.cc
// See: vlc/modules/access/http.c
// 2/21/2012
// Icy, Proxy, and Authentification supports are not implemented.

#include "module/vlccore/http.h"
#include "module/mediacodec/flvcodec.h"
#include "module/mediacodec/mp4codec.h"
#include <QDesktopServices>
#include <QApplication>
#include <QtCore>
#include <QtNetwork>
#include <cstring>

#include <vlc/plugins/vlc_common.h>
extern "C" {
  #include <vlc/src/modules/modules.h>
  module_t *module_find(const char *name);
} // extern "C"

//#define DEBUG "vlccore::http"
#include "module/debug/debug.h"

// - Session -

VlcHttpSession::VlcHttpSession(QObject *parent)
  : Base(parent), state_(Stopped), nam_(0), cookieJar_(0), reply_(0),
    pos_(0), size_(0), ready_(false)
{ }

VlcHttpSession::~VlcHttpSession()
{
  if (nam_)
    delete nam_;
}

bool
VlcHttpSession::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

void
VlcHttpSession::invalidateSize()
{
  if (reply_) {
    size_ = reply_->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (size_ < 0)
      size_ = 0;
  } else
    size_ = 0;
  DOUT("size =" << size_);
}

void
VlcHttpSession::invalidateContentType()
{
  if (reply_)
    contentType_ = reply_->header(QNetworkRequest::ContentTypeHeader).toString();
  else
    contentType_.clear();
  DOUT("contentType =" << contentType_);
}

void
VlcHttpSession::invalidateFileName()
{
  bool mp4 = contentType_.contains("mp4", Qt::CaseInsensitive);
  QString suf = mp4 ? ".mp4" : ".flv";
  QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/Annot";
  fileName_ = dir + "/" + escapeFileName(mediaTitle_) + suf;
}

void
VlcHttpSession::save()
{
  if (fileName_.isEmpty() || buffer_.isEmpty())
    return;
  QFile::remove(fileName_);
  QDir().mkpath(QFileInfo(fileName_).absolutePath());

  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly)) {
    emit error(tr("cannot save to file") + ": " + fileName_);
    return;
  }

  m_.lock();
  file.write(buffer_);
  m_.unlock();
  file.close();

  if (!FlvCodec::isFlvFile(fileName_) && !Mp4Codec::isMp4File(fileName_)) {
    QFile::remove(fileName_);
    emit error(tr("download failed") + ": " + fileName_);
    return;
  }

  emit message(tr("file saved") + ": " + fileName_);
  QApplication::beep();
  emit fileSaved(fileName_);
}

void
VlcHttpSession::finish()
{
  DOUT("enter");
  size_ = buffer_.size();

  if (isRunning()) {
    save();
    setState(Finished);
  }

  if (reply_ && reply_->isRunning())
    reply_->abort();

  emit progress(size_, size_);

  stoppedCond_.wakeAll();
  DOUT("exit");
}

void
VlcHttpSession::stop()
{
  DOUT("enter");
  setState(Stopped);
  DOUT("exit");
}

void
VlcHttpSession::waitForReplyReady()
{
  if (reply_) {
    //DOUT("enter");
    QEventLoop loop;
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    connect(reply_, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply_, SIGNAL(readyRead()), &loop, SLOT(quit()));
    loop.exec();
    //DOUT("exit");
  }
}

bool
VlcHttpSession::tryRedirect()
{
  if (!reply_)
    return false;
  QUrl url = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (url.isEmpty() || url != reply_->url())
    return false;
  url_ = url;
  run();
  return true;
}

void
VlcHttpSession::waitForReady()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (!ready_)
    readyCond_.wait(&m_);
  DOUT("exit");
}

void
VlcHttpSession::waitForStopped()
{
  DOUT("enter");
  QMutexLocker lock(&m_);
  while (isRunning())
    stoppedCond_.wait(&m_);
  DOUT("exit");
}

void
VlcHttpSession::run()
{
  DOUT("enter: url =" << url_);
  setState(Running);

  ready_ = false;
  pos_ = size_ = 0;
  if (!buffer_.isEmpty())
    buffer_.clear();
  contentType_.clear();
  fileName_.clear();

  if (mediaTitle_.isEmpty())
    mediaTitle_ = QDateTime::currentDateTime().toString(Qt::ISODate);

  nam_ = new QNetworkAccessManager;
  if (cookieJar_) {
    DOUT("use cookie jar");
    nam_->setCookieJar(cookieJar_);
  }
  reply_ = nam_->get(QNetworkRequest(url_));
  Q_ASSERT(reply_);
  waitForReplyReady();

  if (tryRedirect()) {
    DOUT("exit: redirected");
    return;
  }

  if (reply_->error() != QNetworkReply::NoError) {
    setState(Error);
    ready_ = true;
    readyCond_.wakeAll();
    stoppedCond_.wakeAll();
    emit error(tr("network error to access URL") + ": " + url_.toString());
    DOUT("exit: network error");
    return;
  }

  if (reply_->isFinished()) {
    m_.lock();
    buffer_ = reply_->readAll();
    m_.unlock();
  }

  invalidateSize();
  invalidateContentType();
  invalidateFileName();

  if (!isMultiMediaMimeType(contentType_)) {
    setState(Error);
    emit error(tr("access forbidden") + ": " + url_.toString());
  }

  DOUT("ready: finished =" << reply_->isFinished() << ", error =" << reply_->error());
  ready_ = true;
  readyCond_.wakeAll();

  while (!reply_->isFinished() &&
         reply_->error() == QNetworkReply::NoError &&
         isRunning()) {
    qint64 count = reply_->bytesAvailable();
    emit progress(buffer_.size(), size_);
    if (count <= 0) {
      waitForReplyReady();
      count = reply_->bytesAvailable();
      if (count <= 0)
        break;
    }
    //DOUT("count =" << count);

    m_.lock();
    buffer_.append(reply_->read(count));
    m_.unlock();
    //DOUT("readyRead");
    readyReadCond_.wakeAll();
  }

  if (isRunning() && reply_->bytesAvailable() > 0) {
    m_.lock();
    buffer_.append(reply_->readAll());
    m_.unlock();
    DOUT("readyRead");
    readyReadCond_.wakeAll();
  }

  finish();

  DOUT("exit");
}

qint64
VlcHttpSession::read(char *data, qint64 maxSize)
{
  if (maxSize <= 0)
    return 0;

  QMutexLocker lock(&m_);
  while (isRunning() &&
         buffer_.size() - pos_ < maxSize)
    readyReadCond_.wait(&m_);

  qint64 ret = qMin(maxSize, buffer_.size() - pos_);
  if (ret) {
    ::memcpy(data, buffer_.constData() + pos_, ret);
    pos_ += ret;
  }
  return ret;
}

bool
VlcHttpSession::seek(qint64 pos)
{
  if (pos < 0)
    return false;
  if (pos == 0)
    return true;
  if (size_ > 0 && (pos >= size_ && pos > buffer_.size()))
    return false;

  m_.lock();
  pos_ = pos;
  m_.unlock();
  return true;
}

QString
VlcHttpSession::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.replace('/', "／");
  ret.replace(':', "-");
  ret.replace('?', "？");
  return ret.trimmed();
}

// - Plugin -

QNetworkCookieJar *VlcHttpPlugin::cookieJar_ = 0;
QString VlcHttpPlugin::mediaTitle_;

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

struct access_sys_t
{ VlcHttpSession session; };

int
VlcHttpPlugin::open(vlc_object_t *p_this)
{
  Q_ASSERT(p_this);
  DOUT("enter");
  access_t *p_access = (access_t *)p_this;

  QString path = QString::fromLocal8Bit(p_access->psz_location).trimmed();
  QString protocal = QString::fromLocal8Bit(p_access->psz_access);
  QString url = protocal + "://" + path;

  if (url.contains(".youtube.com/")) {
    // FIXME: youtube video not saved
    DOUT("exit: youtube URL:" << url);
    return VLC_EGENERIC;
  }

  //STANDARD_READ_ACCESS_INIT;
  ::access_InitFields(p_access);
  ACCESS_SET_CALLBACKS(read, NULL, control, seek); // read, block, control, seek

  access_sys_t *p_sys = p_access->p_sys = new access_sys_t;
  VlcHttpSession &session = p_sys->session;
  connect(&session, SIGNAL(error(QString)), globalInstance(), SIGNAL(error(QString)));
  connect(&session, SIGNAL(message(QString)), globalInstance(), SIGNAL(message(QString)));
  connect(&session, SIGNAL(fileSaved(QString)), globalInstance(), SIGNAL(fileSaved(QString)));
  connect(&session, SIGNAL(progress(qint64,qint64)), globalInstance(), SIGNAL(progress(qint64,qint64)));
  session.setCookieJar(cookieJar_);
  session.setMediaTitle(mediaTitle_);
  session.setUrl(url);
  DOUT("url =" << url);

  //p_access->info.i_size = 0;
  //p_access->info.i_pos = 0;
  //p_access->info.b_eof = false;

  session.start();
  session.waitForReady();

  p_access->info.i_size = session.size();

  bool ok = session.isRunning();
  if (!ok) {
    delete p_sys;
    p_access->p_sys = 0;
  }
  DOUT("exit: ret =" << ok);
  return ok ? VLC_SUCCESS : VLC_EGENERIC;
}

void
VlcHttpPlugin::close(vlc_object_t *p_this)
{
  Q_ASSERT(p_this);
  DOUT("enter");
  access_t *p_access = (access_t *)p_this;
  access_sys_t *p_sys = p_access->p_sys;
  p_access->p_sys = 0;
  if (p_sys) {
    VlcHttpSession &session = p_sys->session;
    if (session.isRunning()) {
      session.stop();
      session.waitForStopped();
      session.wait(5000); // wait at most 5 seconds
    }
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

  p_access->info.i_pos = session.pos();
  p_access->info.b_eof = count != (qint64)i_len;

  DOUT("exit: ret =" << count);
  return count;
}

// Seek: close and re-open a connection at the right place
int
VlcHttpPlugin::seek(access_t *p_access, uint64_t i_pos)
{
  DOUT("enter: i_pos =" << i_pos);

  VlcHttpSession &session = p_access->p_sys->session;
  if (i_pos == (quint64)session.pos()) {
    DOUT("exit: already seeked");
    return true;
  }
  DOUT("size =" << p_access->info.i_size);
  //if (i_pos >= p_access->info.i_size) {
  //  DOUT("exit: ret = egeneric, out of size");
  //  return VLC_EGENERIC;
  //}

  DOUT("availableSize =" << session.availableSize());
  if (i_pos >= (quint64)session.availableSize()) {
    qint64 pos = qMin(session.pos(), session.availableSize() - 1024 * 1024);
    i_pos = pos > 0 ? (quint64)pos : 0;
    DOUT("new pos =" << i_pos);
    //DOUT("exit: ret = egeneric, out of available size");
    //return VLC_EGENERIC;
  }

  bool ok = session.seek(i_pos);
  p_access->info.i_pos = i_pos;

  DOUT("exit: ret =" << ok);
  return ok ? VLC_SUCCESS : VLC_EGENERIC;
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
    pb_bool = (bool *)va_arg(args, bool *);
    //*pb_bool = p_sys->b_seekable;
    *pb_bool = true; // FIXME: not seekable
    break;
  case ACCESS_CAN_FASTSEEK:
    DOUT("query = ACCESS_CAN_FASTSEEK");
    pb_bool = (bool *)va_arg(args, bool *);
    *pb_bool = true;
    break;
  case ACCESS_CAN_PAUSE:
    DOUT("query = ACCESS_CAN_PAUSE");
    pb_bool = (bool *)va_arg(args, bool*);
    *pb_bool = true;
    break;
  case ACCESS_CAN_CONTROL_PACE:
    DOUT("query = ACCESS_CAN_CONTROL_PACE");
    pb_bool = (bool *)va_arg(args, bool *);
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
