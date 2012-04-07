// player.cc
// 6/30/2011

#ifdef _MSC_VER
#  pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
#  pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#ifndef __LIBVLC__
#  define __LIBVLC__ // disable unnecessary warnings
#endif // __LIBVLC__

#include "player.h"
#include "playerprivate.h"
//#include "module/qtext/os.h"
#ifdef WITH_MODULE_VLCCORE
#  include "module/vlccore/video.h"
#  include "module/vlccore/sound.h"
#  ifdef Q_WS_WIN
#    ifdef WITH_WIN_QTWIN
#      include "win/qtwin/qtwin.h"
#    else
#      warning "qtwin is not used"
#    endif // WITH_WIN_QTWIN
#  endif // Q_WS_WIN
#endif // WITH_MODULE_VLCCORE
#ifdef Q_WS_MAC
#  include <QMacCocoaViewContainer>
#endif // Q_WS_MAC
#include <QtGui>
#include <boost/tuple/tuple.hpp>
#include <boost/typeof/typeof.hpp>
#include <cstring>
#include <memory>

#ifndef MODULE_STRING
#  define MODULE_STRING "main"  // needed by VLC
#endif
#include <inttypes.h>
#include <vlc/plugins/vlc_vout.h>
#include <vlc/plugins/vlc_input_item.h>
#include <vlc/lib/libvlc_internal.h>
#include <vlc/lib/media_internal.h>
#include <vlc/lib/media_list_internal.h>
#include <vlc/vlc.h>
//#include <vlc/lib/media_list.c>

//#define DEBUG "player"
#include "module/debug/debug.h"

// - Settings -

namespace { // anonymous

  enum { VOUT_COUNTDOWN = 30 }; // vout countdown timer
  enum { VOUT_TIMEOUT = 500 };  // 0.5 secs

} // anonymous namespace

// - PlayerImpl -

class PlayerImpl
  : public mp_handle_,
    public mp_states_,
    public mp_trackers_
    //public mp_intl_
{ };

// - Event handlers -
// See libvlc_event.h.
//
// libvlc_MediaMetaChanged
// libvlc_MediaSubItemAdded
// libvlc_MediaDurationChanged
// libvlc_MediaParsedChanged
// libvlc_MediaFreed
// libvlc_MediaStateChanged
// libvlc_MediaPlayerMediaChanged
// libvlc_MediaPlayerNothingSpecial
// libvlc_MediaPlayerOpening
// libvlc_MediaPlayerBuffering
// libvlc_MediaPlayerPlaying
// libvlc_MediaPlayerPaused
// libvlc_MediaPlayerStopped
// libvlc_MediaPlayerForward
// libvlc_MediaPlayerBackward
// libvlc_MediaPlayerEndReached
// libvlc_MediaPlayerEncounteredError
// libvlc_MediaPlayerTimeChanged
// libvlc_MediaPlayerPositionChanged
// libvlc_MediaPlayerSeekableChanged
// libvlc_MediaPlayerPausableChanged
// libvlc_MediaPlayerTitleChanged
// libvlc_MediaPlayerSnapshotTaken
// libvlc_MediaPlayerLengthChanged
// libvlc_MediaListItemAdded
// libvlc_MediaListWillAddItem
// libvlc_MediaListItemDeleted
// libvlc_MediaListWillDeleteItem
// libvlc_MediaListViewItemAdded
// libvlc_MediaListViewWillAddItem
// libvlc_MediaListViewItemDeleted
// libvlc_MediaListViewWillDeleteItem
// libvlc_MediaListPlayerPlayed
// libvlc_MediaListPlayerNextItemSet
// libvlc_MediaListPlayerStopped
// libvlc_MediaDiscovererStarted
// libvlc_MediaDiscovererEnded
// libvlc_VlmMediaAdded
// libvlc_VlmMediaRemoved
// libvlc_VlmMediaChanged
// libvlc_VlmMediaInstanceStarted
// libvlc_VlmMediaInstanceStopped
// libvlc_VlmMediaInstanceStatusInit
// libvlc_VlmMediaInstanceStatusOpening
// libvlc_VlmMediaInstanceStatusPlaying
// libvlc_VlmMediaInstanceStatusPause
// libvlc_VlmMediaInstanceStatusEnd
// libvlc_VlmMediaInstanceStatusError
//
// typedef void(* libvlc_callback_t)(const struct libvlc_event_t *, void *);
namespace { // anonymous, vlc event callbacks
  namespace vlc_event_handler_ {
#define VLC_EVENT_HANDLER(_signal) \
    void \
    _signal(const struct libvlc_event_t *event, void *media_player_instance) \
    { \
      DOUT("enter"); \
      Q_UNUSED(event); \
      Player *p = static_cast<Player*>(media_player_instance); \
      Q_ASSERT(p); \
      p->emit_##_signal(); \
      DOUT("exit"); \
    }

    VLC_EVENT_HANDLER(opening)
    VLC_EVENT_HANDLER(buffering)
    VLC_EVENT_HANDLER(playing)
    VLC_EVENT_HANDLER(stopped)
    VLC_EVENT_HANDLER(paused)
    VLC_EVENT_HANDLER(timeChanged)
    //VLC_EVENT_HANDLER(lengthChanged)
    VLC_EVENT_HANDLER(positionChanged)
    VLC_EVENT_HANDLER(mediaChanged)
    //VLC_EVENT_HANDLER(errorEncountered)
    VLC_EVENT_HANDLER(endReached)
#undef MEDIA_PLAYER_EVENT_HANDLER

    void
    lengthChanged(const struct libvlc_event_t *event, void *media_player_instance)
    {
      DOUT("enter");
      Q_UNUSED(event);
      Player *p = static_cast<Player*>(media_player_instance);
      Q_ASSERT(p);
      p->emit_lengthChanged();

      // Addtional operations
      p->invalidateTitleId();

      DOUT("exit");
    }

    void
    errorEncountered(const struct libvlc_event_t *event, void *media_player_instance)
    {
      DOUT("enter");
      Q_UNUSED(event);
      Player *p = static_cast<Player*>(media_player_instance);
      Q_ASSERT(p);
      //p->emit_errorEncountered();
      p->handleError();
      DOUT("exit");
    }
  }
} // anonymous namespace


#ifdef WITH_MODULE_VLCCORE
namespace { // anonymous, vlccore callbacks

  namespace vout_callback_ { // Consisent with vlc_callback_t

    int doubleClickTimeout_ = // time between D and DUD, in msecs
#ifdef WITH_WIN_QTWIN
     QtWin::getDoubleClickInterval() * 1.5
#else
     600
#endif // Q_WS_WIN
    ;

    qint64 recentClickTime_ = 0; // in msecs
    bool ignoreNextMove_ = false;

    // Not used
    //int
    //mouse_clicked(vlc_object_t *p_this, char const *psz_var,
    //              vlc_value_t oldval, vlc_value_t newval, void *p_data)
    //{
    //  qDebug() << "mouse-clicked";
    //  return 0;
    //}

    int
    mouse_moved(vlc_object_t *p_this, char const *psz_var,
                vlc_value_t oldval, vlc_value_t newval, void *p_data )
    {
      //qDebug() << "mouse-moved";
      Q_UNUSED(psz_var); // Q_ASSERT(psz_var == "mouse-moved");
      Q_UNUSED(oldval);

      if (ignoreNextMove_) {
        ignoreNextMove_ = false;
        return VLC_SUCCESS;
      }
      recentClickTime_ = 0;

      Player *player = reinterpret_cast<Player*>(p_data);
      Q_ASSERT(player);
      if (!player || !player->isMouseEventEnabled())
        return 0;
      vout_thread_t *vout = reinterpret_cast<vout_thread_t*>(p_this);
      Q_ASSERT(vout);
      //if (!vout ||
      //    !vout->render.i_width || !vout->render.i_height)
      //  return 0;
      if (!vout)
        return 0;
      QWidget *w = player->impl()->voutWindow();
      if (!w)
        return 0;

      vlccore::vlcbuttons bt = vlccore::vout_mouse_buttons(vout);

      QPoint pos(newval.coords.x, newval.coords.y); // same as libvlc_video_get_cursor
      //QPoint pos = vlccore::vout_map_to_widget(vout, coords, w->size());
      QPoint globalPos =
#ifdef WITH_WIN_QTWIN
          QtWin::getMousePos()
#else
          pos + w->mapToGlobal(QPoint())
#endif // Q_WS_WIN
      ;

      Qt::MouseButton button;
      Qt::MouseButtons buttons;
      boost::tie(button, buttons) = vlccore::vlcbuttons_to_qt(bt);

#ifdef Q_WS_WIN
      // Mouse-move without buttons is already provided by mousehook on Windows
      // Skip to reduce overheads.
      if (!buttons)
        return VLC_SUCCESS;
#endif // Q_WS_WIN

      // Post event across diff threads.
      QCoreApplication::postEvent(w,
        new QMouseEvent(QEvent::MouseMove, pos, globalPos, button, buttons, Qt::NoModifier)
      );

      return VLC_SUCCESS;
    }

    int
    mouse_button_down(vlc_object_t *p_this, char const *psz_var,
                      vlc_value_t oldval, vlc_value_t newval, void *p_data)
    {
      //qDebug() << "mouse-button-down" << newval.i_int;
      Q_UNUSED(psz_var); // Q_ASSERT(psz_var == "mouse-button-down");

      qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

      Player *player = reinterpret_cast<Player*>(p_data);
      Q_ASSERT(player);
      if (!player || !player->isMouseEventEnabled())
        return 0;
      vout_thread_t *vout = reinterpret_cast<vout_thread_t*>(p_this);
      Q_ASSERT(vout);
      if (!vout)
        return 0;
      QWidget *w = player->impl()->voutWindow();
      if (!w)
        return 0;

      QPoint pos = vlccore::vout_mouse_pos(vout); // same as libvlc_video_get_cursor
      //QPoint pos = vlccore::vout_map_to_widget(vout, coords, w->size());
      QPoint globalPos =
#ifdef WITH_WIN_QTWIN
          QtWin::getMousePos()
#else
          pos + w->mapToGlobal(QPoint())
#endif // Q_WS_WIN
      ;

      Qt::MouseButton button;
      Qt::MouseButtons buttons;
      boost::tie(button, buttons) = vlccore::vlcbuttons_to_qt(newval.i_int);

      QEvent::Type type = button ? QEvent::MouseButtonPress : QEvent::MouseButtonRelease;
      if (!button)
        boost::tie(button, buttons) = vlccore::vlcbuttons_to_qt(oldval.i_int);

      if (button != Qt::LeftButton)
        recentClickTime_ = 0;
      else if (type == QEvent::MouseButtonPress) {
        if (recentClickTime_) {
          if (currentTime - recentClickTime_ < doubleClickTimeout_)
            type = QMouseEvent::MouseButtonDblClick;
          recentClickTime_ = 0;
        } else
          recentClickTime_ = currentTime;
      }

#ifdef WITH_WIN_QTWIN
      if (type != QMouseEvent::MouseButtonRelease && button == Qt::LeftButton) {
        // Disable VLC built-in double-click timer.
        ignoreNextMove_ = true;

        static bool rand_;
        rand_ = !rand_;
        int offset = rand_ ? 4 : -4;
        QtWin::sendMouseMove(QPoint(offset, 0), true); // relative == true
      }
#endif // Q_WS_WIN

      // Post event across diff threads.
      QCoreApplication::postEvent(w,
        new QMouseEvent(type, pos, globalPos, button, buttons, Qt::NoModifier)
      );

      return VLC_SUCCESS;
    }

  } // namespace callback_

  ///  Return number of new vouts.
  int
  register_vout_callbacks_(libvlc_media_player_t *mp, Player *player)
  {
    static QList<vout_thread_t*> vouts_;

    Q_ASSERT(mp);
    vout_thread_t **vouts;
    size_t vouts_count;

    vouts = vlccore::GetVouts(mp, &vouts_count);
    if (vouts)
      for (size_t i = 0; i < vouts_count; i++) {
        vout_thread_t *vout = vouts[i];
        Q_ASSERT(vout);
        if (vout && !vouts_.contains(vout)) {
          vouts_.append(vout);
          ::var_AddCallback(vout, "mouse-moved", vout_callback_::mouse_moved, player);
          ::var_AddCallback(vout, "mouse-button-down", vout_callback_::mouse_button_down, player);
          //::var_AddCallback(vout, "mouse-clicked", vout_callback_::mouse_clicked, player);
        }
      }
    return vouts_count;
  }

} // anonymous namespace
#endif // WITH_MODULE_VLCCORE

// - Static properties -

#define SUPPORTED_SUFFICES_(_Type, _TYPE) \
  const QStringList& \
  Player::supported##_Type##Suffices() \
  { \
    static const QStringList ret = \
      QStringList() PLAYER_FORMAT_##_TYPE(<<); \
    return ret; \
  }

  SUPPORTED_SUFFICES_(Video, VIDEO)
  SUPPORTED_SUFFICES_(Audio, AUDIO)
  SUPPORTED_SUFFICES_(Picture, PICTURE)
  SUPPORTED_SUFFICES_(Subtitle, SUBTITLE)
  SUPPORTED_SUFFICES_(Playlist, PLAYLIST)
#undef SUPPORTED_SUFFICES_

#define SUPPORTED_FILTERS_(_Type, _TYPE) \
  const QStringList& \
  Player::supported##_Type##Filters() \
  { \
    static const QStringList ret = \
      QStringList() PLAYER_FORMAT_##_TYPE(<< "*."); \
    return ret; \
  }

  SUPPORTED_FILTERS_(Video, VIDEO)
  SUPPORTED_FILTERS_(Audio, AUDIO)
  SUPPORTED_FILTERS_(Picture, PICTURE)
  SUPPORTED_FILTERS_(Subtitle, SUBTITLE)
  SUPPORTED_FILTERS_(Playlist, PLAYLIST)
#undef SUPPORTED_FILTERS_

#define IS_SUPPORTED_(_type) \
  bool \
  Player::isSupported##_type(const QString &fileName) \
  { \
    foreach (QString suffix, supported##_type##Suffices()) \
      if (fileName.endsWith("." + suffix, Qt::CaseInsensitive)) \
        return true; \
    return false; \
  }

  IS_SUPPORTED_(Video)
  IS_SUPPORTED_(Audio)
  IS_SUPPORTED_(Picture)
  IS_SUPPORTED_(Subtitle)
  IS_SUPPORTED_(Playlist)
#undef IS_SUPPORTED_

// - Constructions -

Player::Player(QObject *parent)
  : Base(parent), impl_(0)
{
  DOUT("enter");
#ifdef WITH_MODULE_VLCCORE
  connect(VlcHttpPlugin::globalInstance(), SIGNAL(error(QString)), SIGNAL(error(QString)));
  connect(VlcHttpPlugin::globalInstance(), SIGNAL(message(QString)), SIGNAL(message(QString)));
  connect(VlcHttpPlugin::globalInstance(), SIGNAL(fileSaved(QString)), SIGNAL(fileSaved(QString)));
  connect(VlcHttpPlugin::globalInstance(), SIGNAL(progress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));
#endif // WITH_MODULE_VLCCORE
  DOUT("exit");
}

Player::~Player()
{
  DOUT("enter");
  destroy();
  DOUT("exit");
}

bool
Player::isValid() const
{ return impl_ && impl_->valid(); }

void
Player::destroy()
{
  if (impl_) {
    //if (impl_->codec())
    //  delete impl_->codec();
    delete impl_;
  }
}

void
Player::reset()
{
  DOUT("enter");

  destroy();

  impl_ = new Impl;
  impl_->reset();
  Q_ASSERT(isValid());

  enum { initial_volume = int(0.5 * VLC_MAX_VOLUME) };
  ::libvlc_audio_set_volume(impl_->player(), initial_volume);

  setUserAgent();

  attachEvents();

  // Set timer.
  //impl_->setUpdateTimer(new QTimer(this));
  //connect(impl_->updateTimer(), SIGNAL(timeout()), SLOT(update()));
  //setUpdateInterval();

  DOUT("exit");
}

#define REGISTER_EVENTS(_attach) \
  void \
  Player::_attach##Events() \
  { \
    libvlc_event_manager_t *event_manager = ::libvlc_media_player_event_manager(impl_->player()); \
    Q_ASSERT(event_manager); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerOpening, vlc_event_handler_::opening, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerBuffering, vlc_event_handler_::buffering, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerPlaying, vlc_event_handler_::playing, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerPaused, vlc_event_handler_::paused, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerStopped, vlc_event_handler_::stopped, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerTimeChanged, vlc_event_handler_::timeChanged, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerLengthChanged, vlc_event_handler_::lengthChanged, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerPositionChanged, vlc_event_handler_::positionChanged, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerMediaChanged, vlc_event_handler_::mediaChanged, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerEncounteredError, vlc_event_handler_::errorEncountered, this); \
    ::libvlc_event_##_attach(event_manager, libvlc_MediaPlayerEndReached, vlc_event_handler_::endReached, this); \
  }

  REGISTER_EVENTS(attach)
  REGISTER_EVENTS(detach)
#undef REGISTER_EVENTS

// - Embedding -

void
Player::setEmbeddedWindow(WindowHandle handle)
{
  Q_ASSERT(isValid());
  Q_ASSERT(handle);
  if (handle) {
    impl_->setEmbedded();
    ::libvlc_media_player_set_drawable(impl_->player(), handle);
  }
}

Player::WindowHandle
Player::embeddedWindow() const
{
  Q_ASSERT(isValid());
  return static_cast<WindowHandle>(
    ::libvlc_media_player_get_drawable(impl_->player())
  );
}

#ifdef Q_WS_MAC
void
Player::embed(QMacCocoaViewContainer *w)
{
  DOUT("enter");
  Q_ASSERT(isValid());
  Q_ASSERT(w);
  if (w)
    setEmbeddedWindow(w->cocoaView());
  impl_->setVoutWindow(w);
  DOUT("exit");
}
#else
void
Player::embed(QWidget *w)
{
  DOUT("enter");
  Q_ASSERT(isValid());
  Q_ASSERT(w);
  if (w)
    setEmbeddedWindow(w->winId());

  impl_->setVoutWindow(w);
  DOUT("exit");
}
#endif // Q_WS_MAC

bool
Player::isEmbedded() const
{
  Q_ASSERT (isValid());
  return impl_->isEmbedded();
}

// - Encoding -
//QString
//Player::encoding() const
//{
//  Q_ASSERT(isValid());
//  return impl_->codec()->encoding();
//}
//
//void
//Player::setEncoding(const QString &encoding)
//{
//  Q_ASSERT(isValid());
//
//  if (impl_->codec())
//    delete impl_->codec();
//  impl_->setCodec(new Core::TextCodec(encoding));
//  emit encodingChanged();
//}

//QByteArray
//Player::decode(const QString &input) const
//{
//  //return impl_ && impl_->codec() ?
//  //  impl_->codec()->encode(input) :
//  return input.toAscii();
//}

//QString
//Player::encode(const char *input) const
//{
//  //return impl_ && impl_->codec() ?
//  //  impl_->codec()->decode(input) :
//  return QString(input);
//}

// - Playing states -
bool
Player::isPlaying() const
{ return isValid() && ::libvlc_media_player_is_playing(impl_->player()); }

bool
Player::isPaused() const
{ return isValid() && impl_->isPaused(); }

bool
Player::isStopped() const
{ return !isPlaying() && !isPaused(); }

Player::Status
Player::status() const
{
  return !isValid() ? Stopped :
         isPlaying() ? Playing :
         isPaused() ? Paused :
         Stopped;
}

bool
Player::pausable() const
{ return hasMedia() && ::libvlc_media_player_can_pause(impl_->player()); }

bool
Player::seekable() const
{ return hasMedia() && ::libvlc_media_player_is_seekable(impl_->player()); }

// - Open media -

void
Player::clearMedia()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  if (hasMedia())
    closeMedia();
  DOUT("exit");
}

bool
Player::openMediaAsCD(const QString &path)
{
  QString mrl = path;
  if (!mrl.startsWith(PLAYER_URL_CD, Qt::CaseInsensitive))
    mrl = PLAYER_URL_CD + mrl;
  if (mrl.endsWith('/'))
    mrl[mrl.length() - 1] = '\\';
  return openMedia(mrl);
}

void
Player::setStream(const QStringList &mrls, qint64 duration)
{
#ifdef WITH_MODULE_VLCCORE
  VlcHttpPlugin:: setUrls(mrls);
  VlcHttpPlugin:: setDuration(duration);
#else
  Q_UNUSED(mrls)
  Q_UNUSED(duration)
#endif // WITH_MODULE_VLCCORE
}

bool
Player::openStream(const QStringList &mrls)
{
  if (hasMedia())
    closeMedia();
  if (mrls.isEmpty())
    return false;
  setStream(mrls);
  return openMedia(mrls.first());
}

bool
Player::openMedia(const QString &path)
{
  DOUT("enter: path =" << path);
  Q_ASSERT(isValid());
  if (hasMedia())
    closeMedia();

  DOUT("open:" << path);

  // Handle CDA
  if (path.endsWith(".cda", Qt::CaseInsensitive)) {
    QFileInfo fi(path);
    QRegExp rx("^Track(\\d+)\\.cda$", Qt::CaseInsensitive);
    if (rx.indexIn(fi.fileName()) >= 0) {
      bool ok;
      int track1 = rx.cap(1).toInt(&ok);
      if (ok && track1 >= 1) {
        impl_->setTrackNumber(track1 - 1);
        bool ret = openMediaAsCD(fi.absolutePath());
        DOUT("exit from cda code path: ret =" << ret);
        return ret;
      }
    }
  }

  if (isSupportedPlaylist(path)) {
#ifdef USE_PLAYER_PLAYLIST
    QList<libvlc_media_t*> l = parsePlaylist(path);
    if (l.isEmpty()) {
      DOUT("empty play list");
      emit errorEncountered();
      DOUT("exit: ret = false");
      return false;
    }

    impl_->setMediaList(l);
    setTrackNumber(impl_->trackNumber());
    DOUT("exit: ret = true");
    return true;
#else
    DOUT("exit: playlist support is not enabled");
    return false;
#endif // USE_PLAYER_PLAYLIST
  }

  libvlc_media_t *md = path.contains("://") ?
    ::libvlc_media_new_location(impl_->instance(), _cs(path)) :   // MRL
    ::libvlc_media_new_path(impl_->instance(), vlcpath(path)); // local file
  impl_->setMedia(md);

  //libvlc_media_list_player_t *mlp = libvlc_media_list_player_new(impl_->instance());
  //libvlc_media_list_player_set_media_player(mlp, impl_->player());
  //libvlc_media_list_t *ml = ::libvlc_media_list_new(impl_->instance());
  //libvlc_media_list_add_media(ml, impl_->media());
  //libvlc_media_list_add_file_content(ml, "d:/devel/mp4.mp4");
  //libvlc_media_list_add_file_content(ml, "d:/devel/mp4.mp4");
  //libvlc_media_list_player_set_media_list(mlp, ml);

  if (!impl_->media()) {
    impl_->setMediaPath();
    DOUT("WARNING: failed");
    DOUT("exit: ret = false");
    return false;
  }

  impl_->setMediaPath(path);


  ::libvlc_media_player_set_media(impl_->player(), impl_->media());

  if (isMouseEventEnabled())
    startVoutTimer();

  DOUT("exit: ret = true");
  return true;
}

void
Player::closeMedia()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  if (isMouseEventEnabled())
    stopVoutTimer();
  if (!isStopped()) {
    stop();
    //enum { timeout = 1000 };
    //qDebug() << "Player::closeMedia: enter sleep: timeout =" << timeout;
    //QtExt::sleep(timeout);
    //qDebug() << "Player::closeMedia: leave sleep";
    //DOUT("processEvent: enter");
    //enum { timeout = 1000 };
    //qApp->processEvents(QEventLoop::AllEvents, timeout);
    //DOUT("processEvent: exit");
  }

  impl_->setPaused(false);
  impl_->setSubtitleId();
  impl_->setTitleId();
  impl_->setMediaPath();
  impl_->setMediaTitle();
  impl_->setTrackNumber();
  impl_->setExternalSubtitles();

#ifdef WITH_MODULE_VLCCORE
  VlcHttpPlugin::closeSession();
  VlcHttpPlugin::setMediaTitle(QString());
  VlcHttpPlugin::setUrls(QStringList());
  VlcHttpPlugin::setDuration(0);
#endif // WITH_MODULE_VLCCORE

  if (!impl_->mediaList().isEmpty()) {
    foreach (libvlc_media_t *m, impl_->mediaList())
      if (m && m != impl_->media())
        ::libvlc_media_release(m);
    impl_->setMediaList();
  }

  if (impl_->media()) {
    ::libvlc_media_release(impl_->media());
    impl_->setMedia();
  }
  ::libvlc_media_player_set_media(impl_->player(), 0); // If any, previous md will be released.

  emit mediaClosed();
  DOUT("exit");
}

bool
Player::hasMedia() const
{
  return isValid() && impl_->media();
         //::libvlc_media_player_get_media(impl_->player()); // avoid parallel contention
}

bool
Player::hasRemoteMedia() const
{ return hasMedia() && mediaPath().contains("p://"); }

// - Full screen -
void
Player::setFullScreen(bool t)
{
  Q_ASSERT(isValid());
  ::libvlc_set_fullscreen(impl_->player(), t);
}

bool
Player::isFullScreen() const
{
  Q_ASSERT(isValid());
  return ::libvlc_get_fullscreen(impl_->player());
}

void
Player::toggleFullScreen()
{
  Q_ASSERT(isValid());
  ::libvlc_toggle_fullscreen(impl_->player());
}

// - Media information -

QString
Player::mediaTitle() const
{
  //Q_ASSERT(isValid());
  if (!impl_->mediaTitle().isEmpty())
    return impl_->mediaTitle();

  if (!hasMedia())
    return QString();

  const char *title = ::libvlc_media_get_meta(impl_->media(), libvlc_meta_Title);
  // VLC i18n bug. It simpily cannot handle UTF-8 correctly.
  if (!title || ::strstr(title, "??"))
    return QString();

  return _qs(title);
}

QString
Player::mediaPath() const
{
  Q_ASSERT(isValid());
  return impl_->mediaPath();
}

Player::MediaInfo
Player::mediaInfo() const
{
  MediaInfo ret;
  if (hasMedia()) {
    ret.path = mediaPath();
    ret.title = mediaTitle();
    ret.track = trackNumber();
  }
  return ret;
}

int
Player::trackNumber() const
{ return impl_->trackNumber(); }

int
Player::trackCount() const
{ return impl_->mediaList().size(); }

bool
Player::hasPlaylist() const
{ return !impl_->mediaList().isEmpty(); }

QList<Player::MediaInfo>
Player::playlist() const
{
  QList<MediaInfo> ret;
  int i = 0;
  foreach (libvlc_media_t *md, impl_->mediaList()) {
    MediaInfo mi;
    mi.track = i++;

    const char *mrl = ::libvlc_media_get_mrl(md);
    if (mrl)
      mi.path = mrl;

    const char *title = ::libvlc_media_get_meta(md, libvlc_meta_Title);
    if (title && !::strstr(title, "??"))
      mi.title = title;

    ret.append(mi);
  }
  return ret;
}

// - Play control -

void
Player::play()
{
  DOUT("enter");
  if (hasMedia()) {
    impl_->setPaused(false);
    ::libvlc_media_player_play(impl_->player());
  }
  DOUT("exit");
}

void
Player::stop()
{
  DOUT("enter");
  if (hasMedia()) {
    emit stopping();
    impl_->setPaused(false);
    ::libvlc_media_player_stop(impl_->player());
  }
  DOUT("exit");
}

void
Player::pause()
{
  DOUT("enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(true);
    ::libvlc_media_player_set_pause(impl_->player(), true);
  }
  DOUT("exit");
}

void
Player::resume()
{
  DOUT("enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(false);
    ::libvlc_media_player_set_pause(impl_->player(), false);
  }
  DOUT("exit");
}

void
Player::playPause()
{
  DOUT("enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(!impl_->isPaused());
    ::libvlc_media_player_pause(impl_->player());
  }
  DOUT("exit");
}

void
Player::nextFrame()
{
  DOUT("enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused();
    ::libvlc_media_player_next_frame(impl_->player());
  }
  DOUT("exit");
}

bool
Player::snapshot(const QString &path)
{
  DOUT("enter");
  if (!hasMedia()) {
    DOUT("no media, ignored");
    return false;
  }

  int err = ::libvlc_video_take_snapshot(impl_->player(), 0, vlcpath(path), 0, 0);
  DOUT("exit: ret =" << !err);
  return !err;
}


// - Set/get properties -

void
Player::setMouseEnabled(bool enable)
{
  Q_ASSERT(isValid());
  impl_->setMouseEnabled(enable);
  ::libvlc_video_set_mouse_input(impl_->player(), enable ? 1 : 0);
}

bool
Player::isMouseEnabled() const
{
  Q_ASSERT(isValid());
  return impl_->isMouseEnabled();
}

void
Player::setKeyboardEnabled(bool enable)
{
  Q_ASSERT(isValid());
  impl_->setKeyEnabled(enable);
  ::libvlc_video_set_key_input(impl_->player(), enable ? 1 : 0);
}

bool
Player::isKeyboardEnabled() const
{
  Q_ASSERT(isValid());
  return impl_->isKeyEnabled();
}

void
Player::mute()
{ setVolume(0); }

void
Player::setVolume(qreal vol)
{
  DOUT("enter");
  Q_ASSERT(isValid());
  if (impl_) {
    int newv = vol * VLC_MAX_VOLUME;
    int oldv = ::libvlc_audio_get_volume(impl_->player());
    if (oldv != newv) {
      ::libvlc_audio_set_volume(impl_->player(), newv);
      emit volumeChanged();
    } else if (newv == 0 || newv == VLC_MAX_VOLUME)
      emit volumeChanged();
  }
  DOUT("exit");
}

void
Player::setPosition(qreal pos, bool checkPos)
{
  DOUT("enter");
  if (hasMedia()) {
    if (checkPos) {
      double max = availablePosition();
      if (!qFuzzyCompare(1 + max, 1) && pos > max) {
        emit error(tr("seek too much"));
        return;
      }
    }
    ::libvlc_media_player_set_position(impl_->player(), pos);
  }
  DOUT("exit");
}

QString
Player::aspectRatio() const
{
  Q_ASSERT(isValid());
  char *p = ::libvlc_video_get_aspect_ratio(impl_->player());
  QString ret;
  if (p) {
    ret = _qs(p);
    ::libvlc_free(p);
  }
  return ret;
}

void
Player::setAspectRatio(const char *ratio)
{
  Q_ASSERT(isValid());
  ::libvlc_video_set_aspect_ratio(impl_->player(), ratio);
  emit aspectRatioChanged(_qs(ratio));
}
void
Player::setAspectRatio(const QString &ratio)
{
  Q_ASSERT(isValid());
  if (ratio.isEmpty())
    ::libvlc_video_set_aspect_ratio(impl_->player(), 0);
  else
    ::libvlc_video_set_aspect_ratio(impl_->player(), _cs(ratio));
  emit aspectRatioChanged(ratio);
}

void
Player::clearAspectRatio()
{
  Q_ASSERT(isValid());
  ::libvlc_video_set_aspect_ratio(impl_->player(), 0);
  emit aspectRatioChanged(QString());
}

void
Player::setAspectRatio(int width, int height)
{
  if (width > 0 && height > 0)
    setAspectRatio(QString("%1:%2").arg(QString::number(width)).arg(QString::number(height)));
  else
    clearAspectRatio();

}

void
Player::setRate(qreal rate)
{
  Q_ASSERT(isValid());
  ::libvlc_media_player_set_rate(impl_->player(), rate);
  emit rateChanged(rate);
}

qreal
Player::rate() const
{
  Q_ASSERT(isValid());
  return ::libvlc_media_player_get_rate(impl_->player());
}

qreal
Player::volume() const
{
  Q_ASSERT(isValid());
  return ::libvlc_audio_get_volume(impl_->player()) / (qreal)VLC_MAX_VOLUME;
}

qreal
Player::position() const
{ return hasMedia() ? ::libvlc_media_player_get_position(impl_->player()) : 0; }

qreal
Player::availablePosition() const
{
  if (!hasMedia())
    return 0;
  //if (!hasRemoteMedia())
  //  return 0;

#ifdef WITH_MODULE_VLCCORE
  qint64 duration, size;
  if ((duration = VlcHttpPlugin::duration()) > 0) {
    qint64 ts = VlcHttpPlugin::availableDuration();
    return ts / (double)duration;
  } else if ((size = VlcHttpPlugin::size()) > 0) {
    qint64 sz = VlcHttpPlugin::availableSize();
    return sz / (double)size;
  } else
#endif // WITH_MODULE_VLCCORE
    return 0;
}

// - Play time -

qint64
Player::mediaLength() const
{
  Q_ASSERT(isValid());
  return ::libvlc_media_player_get_length(impl_->player());
}

qint64
Player::time() const
{
  Q_ASSERT(isValid());
  return ::libvlc_media_player_get_time(impl_->player());
}

void
Player::setTime(qint64 time)
{
  DOUT("enter");
  Q_ASSERT(isValid());
  if (hasMedia())
    ::libvlc_media_player_set_time(impl_->player(), time);
  DOUT("exit");
}

// - Subtitles -

int
Player::subtitleCount() const
{
  Q_ASSERT(isValid());
  return hasMedia() ? ::libvlc_video_get_spu_count(impl_->player())
                    : 0;
}

bool
Player::hasSubtitles() const
{
  Q_ASSERT(isValid());
  return subtitleCount() > 0;
}

QStringList
Player::subtitleDescriptions() const
{
  Q_ASSERT(isValid());
  QStringList ret;

  libvlc_track_description_t *first = ::libvlc_video_get_spu_description(impl_->player());

  /// Skip first disabled track.
  if (first)
    first = first->p_next;

  while (first) {
    if (first->psz_name)
      ret.append(first->psz_name);
    else
      ret.append(QString());
    first = first->p_next;
  }

  return ret;
}

int
Player::subtitleId() const
{
  Q_ASSERT(isValid());
  int id = ::libvlc_video_get_spu(impl_->player());
  return id > 0 ? id - 1 :
         impl_->subtitleId() - 1;
}

bool
Player::isSubtitleVisible() const
{
  Q_ASSERT(isValid());
  return ::libvlc_video_get_spu(impl_->player()) > 0;
}

void
Player::setSubtitleVisible(bool visible)
{
  Q_ASSERT(isValid());
  if (visible) {
    ::libvlc_video_set_spu(impl_->player(), impl_->subtitleId());
  } else {
    int bak = ::libvlc_video_get_spu(impl_->player());
    if (bak > 0) {
      impl_->setSubtitleId(bak);
      ::libvlc_video_set_spu(impl_->player(), 0);
    }
  }
}

void
Player::showSubtitle()
{ setSubtitleVisible(true); }

void
Player::hideSubtitle()
{ setSubtitleVisible(false); }

void
Player::setSubtitleId(int id)
{
  Q_ASSERT(isValid());
  Q_ASSERT(0 <= id && id < subtitleCount());
  bool err = ::libvlc_video_set_spu(impl_->player(), id + 1);
  if (!err) {
    impl_->setSubtitleId(id + 1);
    emit subtitleChanged();
  } else
    DOUT("failed: id =" << id);
}

bool
Player::openSubtitle(const QString &fileName)
{ return setSubtitleFromFile(fileName); }

bool
Player::setSubtitleFromFile(const QString &fileName)
{
  DOUT("enter:" << fileName);
  Q_ASSERT(isValid());

  QString path = fileName;
#ifdef Q_WS_WIN
  path.replace('/', '\\');
#endif // Q_WS_WIN

  DOUT("opening subtitle:" << path);
  if (impl_->externalSubtitles().indexOf(path) >= 0) {
    DOUT("subtitle already loaded");
    DOUT("exit");
    return true;
  }

  bool ok = ::libvlc_video_set_subtitle_file(impl_->player(), vlcpath(path));
  if (ok) {
    DOUT("succeeded, number of subtitles (0 for the first time):" << ::libvlc_video_get_spu_count(impl_->player()));
    impl_->externalSubtitles() << path;
    impl_->setSubtitleId(::libvlc_video_get_spu_count(impl_->player()) - 1);
    emit subtitleChanged();
  } else
    DOUT("failed");

  DOUT("exit");
  return ok;
}

void
Player::addSubtitleFromFile(const QString &fileName)
{
  Q_ASSERT(isValid());
  int bak = ::libvlc_video_get_spu(impl_->player());
  if (bak > 0)
    impl_->setSubtitleId(bak);
  ::libvlc_video_set_subtitle_file(impl_->player(), vlcpath(fileName));
  ::libvlc_video_set_spu(impl_->player(), impl_->subtitleId());
}

void
Player::loadExternalSubtitles()
{
  QStringList subtitles = searchExternalSubtitles();
  if (!subtitles.empty()) {
    foreach (const QString &path, subtitles)
      openSubtitle(path);

    if (hasSubtitles())
      setSubtitleId(0);
  }
}

QStringList
Player::searchExternalSubtitles() const
{
  DOUT("enter");
  QStringList ret;
  if (hasMedia()) {
    QString fileName = mediaPath();
    QDir dir = QFileInfo(fileName).absolutePath();
    DOUT("search in directory:" << dir.path());
    if (dir.exists()) {
      QString baseName = QFileInfo(fileName).baseName() ;
      QStringList filters = supportedSubtitleFilters();
      foreach (QString f, dir.entryList(filters, QDir::Files))
        if (f.startsWith(baseName + "."))
          ret << dir.path() + "/" + f;
    } else
      DOUT("directory to search not existed");
  }
  DOUT("found subtitles count:" << ret.size());
  DOUT("exit");
  return ret;
}

// - Title/chapter -

bool
Player::hasTitles() const
{ return titleCount() > 0; }

int
Player::titleId() const
{
  DOUT("enter");
  int ret = 0;
  if (hasMedia())
    ret = ::libvlc_media_player_get_title(impl_->player());
  DOUT("exit: ret =" << ret);
  return ret;
}

int
Player::titleCount() const
{
  DOUT("enter");
  int ret = 0;
  if (hasMedia())
    ret = ::libvlc_media_player_get_title_count(impl_->player());
  DOUT("exit: ret =" << ret);
  return ret;
}

void
Player::setTitleId(int tid)
{
  DOUT("enter: tid =" << tid);
  if (hasMedia()) {
    ::libvlc_media_player_set_title(impl_->player(), tid);
    invalidateTitleId();
  }
  DOUT("exit");
}

void
Player::setPreviousTitle()
{
  DOUT("enter");
  if (hasMedia()) {
    int tid = titleId();
    if (tid > 0)
      setTitleId(tid - 1);
  }
  DOUT("exit");
}

void
Player::setNextTitle()
{
  DOUT("enter");
  if (hasMedia()) {
    int tid = titleId();
    if (tid < titleCount() - 1)
      setTitleId(tid + 1);
  }
  DOUT("exit");
}

void
Player::invalidateTitleId()
{
  DOUT("ienter");
  if (hasMedia()) {
    int tid = titleId();
    if (tid != impl_->titleId()) {
      impl_->setTitleId(tid);
      emit titleIdChanged(tid);
    }
  }
  DOUT("exit");
}

bool
Player::hasChapters() const
{ return chapterCount() > 0; }

int
Player::chapterId() const
{
  DOUT("enter");
  int ret = 0;
  if (hasMedia())
    ret = ::libvlc_media_player_get_chapter(impl_->player());
  DOUT("exit: ret =" << ret);
  return ret;
}

int
Player::chapterCount() const
{
  DOUT("enter");
  int ret = 0;
  if (hasMedia())
    ret = ::libvlc_media_player_get_chapter_count(impl_->player());
  DOUT("exit: ret =" << ret);
  return ret;
}

int
Player::chapterCountForTitleId(int tid) const
{
  DOUT("enter: tid =" << tid);
  int ret = 0;
  if (hasMedia())
    ret = ::libvlc_media_player_get_chapter_count_for_title(impl_->player(), tid);
  DOUT("exit: ret =" << ret);
  return ret;
}

void
Player::setChapterId(int cid)
{
  DOUT("enter: cid =" << cid);
  if (hasMedia())
    ::libvlc_media_player_set_chapter(impl_->player(), cid);
  DOUT("exit");
}

void
Player::setPreviousChapter()
{
  DOUT("enter");
  if (hasMedia())
    ::libvlc_media_player_previous_chapter(impl_->player());
  DOUT("exit");
}

void
Player::setNextChapter()
{
  DOUT("enter");
  if (hasMedia())
    ::libvlc_media_player_next_chapter(impl_->player());
  DOUT("exit");
}

// - vlccore -

bool
Player::isMouseEventEnabled() const
{ return impl_->isMouseEventEnabled(); }

void
Player::setMouseEventEnabled(bool enabled)
{
  impl_->setMouseEventEnabled(enabled);
  //if (enabled)
  //  startVoutTimer();
  //else
  //  stopVoutTimer();
}

void
Player::startVoutTimer()
{
  QtExt::CountdownTimer *timer = impl_->voutCountdown();
  if (!timer) {
    impl_->setVoutCountdown(
      timer = new QtExt::CountdownTimer(this)
    );
    timer->setInterval(VOUT_TIMEOUT);
    connect(timer, SIGNAL(timeout()), SLOT(invalidateVout()));
  }

  if (!timer->isActive())
    timer->start(VOUT_COUNTDOWN);
  else
    timer->setCount(VOUT_COUNTDOWN);
}

void
Player::stopVoutTimer()
{
  if (impl_->voutCountdown())
    impl_->voutCountdown()->stop();
}

void
Player::invalidateVout()
{
  if (hasMedia()) {
#ifdef WITH_MODULE_VLCCORE
    bool ok = ::register_vout_callbacks_(impl_->player(), this);
    if (ok) // new vout found
#endif // WITH_MODULE_VLCCORE
      stopVoutTimer();
  }
}

// - Error handling -

void
Player::handleError()
{
  // Handle subitems of CDDA.
  // See: http://forum.videolan.org/viewtopic.php?f=32&t=88981
  if (impl_->media() &&
      ::libvlc_media_subitems(impl_->media())) {

    QList<libvlc_media_t*> l = parsePlaylist(impl_->media());
    if (!l.isEmpty()) {
      impl_->setMediaList(l);
      //setTrackNumber(impl_->trackNumber());
      QTimer::singleShot(0, this, SLOT(setTrackNumber()));
      return;
    }
  }

  emit stopped();
  //closeMedia();
  emit mediaClosed();

  emit errorEncountered();
}

// - Playlist -

QList<libvlc_media_t*>
Player::parsePlaylist(const QString &fileName) const
{
  QList<libvlc_media_t*> ret;
#ifdef USE_PLAYER_PLAYLIST
  QFileInfo fi(fileName);
  if (!fi.exists())
    return ret;

  // jichi 11/30/2011 FIXME: ml never released that could cause runtime memory leak.
  libvlc_instance_t *parent = const_cast<libvlc_instance_t*>(impl_->instance());
  libvlc_media_list_t *ml = ::libvlc_media_list_new(parent);

  ::libvlc_media_list_add_file_content(ml, vlcpath(fileName)); // FIXME: deprecated in VLC 1.1.11
  int count = ::libvlc_media_list_count(ml);
  for (int i = 0; i < count; i++) { // count should always be 1 if succeed
    libvlc_media_t *md = ::libvlc_media_list_item_at_index(ml, i);
    Q_ASSERT(md);
    ret.append(parsePlaylist(md));
  }

  ::libvlc_media_list_release(ml);

#else
  Q_UNUSED(fileName);
#endif // USE_PLAYER_PLAYLIST
  return ret;
}

QList<libvlc_media_t*>
Player::parsePlaylist(libvlc_media_t *md) const
{
  QList<libvlc_media_t*> ret;
  Q_ASSERT(md);
  if (!md)
    return ret;

  libvlc_media_list_t *ml = ::libvlc_media_subitems(md);
  if (ml) {
    int count = ::libvlc_media_list_count(ml);
    for (int i = 0; i < count; i++)
      ret.append(
        ::libvlc_media_list_item_at_index(ml, i)
      );
  }

  return ret;
}

void
Player::setTrackNumber()
{ setTrackNumber(impl_->trackNumber()); }

void
Player::setTrackNumber(int track)
{
  Q_ASSERT(isValid());
  if (track < 0 || track >= trackCount())
    return;

  if (hasMedia())
    stop();

  impl_->setTrackNumber(track);
  impl_->setMedia(impl_->mediaList().at(track));
  const char *mrl = ::libvlc_media_get_mrl(impl_->media());
  impl_->setMediaPath(_qs(mrl));

  ::libvlc_media_player_set_media(impl_->player(), impl_->media());

  if (isMouseEventEnabled())
    startVoutTimer();
  emit trackNumberChanged(track);
}

void
Player::nextTrack()
{
  int track = impl_->trackNumber() + 1;
  if (track < 0 || track >= trackCount())
    track = 0;

  setTrackNumber(track);
}

void
Player::previousTrack()
{
  int track = impl_->trackNumber() - 1;
  if (track < 0 || track >= trackCount())
    track = 0;

  setTrackNumber(track);
}

// - Audio tracks -

int
Player::audioTrackCount() const
{
  Q_ASSERT(isValid());
  int ret = ::libvlc_audio_get_track_count(impl_->player()) - 1;
  if (ret < 0)
    ret = 0;
  return ret;
}

bool
Player::hasAudioTracks() const
{
  Q_ASSERT(isValid());
  return audioTrackCount() > 1;
}

QStringList
Player::audioTrackDescriptions() const
{
  Q_ASSERT(isValid());
  QStringList ret;

  libvlc_track_description_t *first = ::libvlc_audio_get_track_description(impl_->player());

  /// Skip first disabled track.
  if (first)
    first = first->p_next;

  while (first) {
    ret.append(_qs(first->psz_name));
    first = first->p_next;
  }

  return ret;
}

int
Player::audioTrackId() const
{
  Q_ASSERT(isValid());
  int ret = ::libvlc_audio_get_track(impl_->player()) - 1;
  if (ret < 0)
    ret = 0;
  return ret;
}

void
Player::setAudioTrackId(int id)
{
  Q_ASSERT(isValid());
  Q_ASSERT(0 <= id && id < audioTrackCount());
  bool ok = ::libvlc_audio_set_track(impl_->player(), id + 1);
  if (ok)
    emit audioTrackChanged();
}

// - User agent -

QString
Player::defaultUserAgent()
{ return PLAYER_USER_AGENT; }

QString
Player::userAgent() const
{
  Q_ASSERT(isValid());
  return impl_->userAgent();
}

void
Player::setUserAgent(const QString &agent)
{
  Q_ASSERT(isValid());
  if (agent.isEmpty()) {
    impl_->setUserAgent(PLAYER_USER_AGENT);
    ::libvlc_set_user_agent(impl_->instance(), PLAYER_USER_AGENT, PLAYER_USER_AGENT);
  } else {
    impl_->setUserAgent(agent);
    ::libvlc_set_user_agent(impl_->instance(), PLAYER_USER_AGENT, _cs(impl_->userAgent()));
  }
}

void
Player::setCookieJar(QNetworkCookieJar *jar)
{
#ifdef WITH_MODULE_VLCCORE
  VlcHttpPlugin::setCookieJar(jar);
#endif // WITH_MODULE_VLCCORE
}

bool
Player::isBufferSaved() const
{
#ifdef WITH_MODULE_VLCCORE
  return VlcHttpPlugin::isBufferSaved();
#endif // WITH_MODULE_VLCCORE
}

bool
Player::isDownloadFinished() const
{
#ifdef WITH_MODULE_VLCCORE
  return VlcHttpPlugin::isFinished();
#endif // WITH_MODULE_VLCCORE
}

void
Player::setBufferSaved(bool t)
{
#ifdef WITH_MODULE_VLCCORE
  VlcHttpPlugin::setBufferSaved(t);
#endif // WITH_MODULE_VLCCORE
}

void
Player::saveBuffer()
{
#ifdef WITH_MODULE_VLCCORE
  VlcHttpPlugin::save();
#endif // WITH_MODULE_VLCCORE
}

// - Title -

void
Player::setMediaTitle(const QString &t)
{
  Q_ASSERT(isValid());
  if (t != impl_->mediaTitle()) {
    impl_->setMediaTitle(t);
#ifdef WITH_MODULE_VLCCORE
    VlcHttpPlugin::setMediaTitle(t);
#endif // WITH_MODULE_VLCCORE
    emit mediaTitleChanged(t);
  }
}

// - Dispose

void
Player::dispose()
{
  //if (impl_)
  //  detachEvents();

  if (hasMedia()) {
    if (!isStopped()) {
      mute();
      stop();
    }
    closeMedia();
  }
}

// - Adjustment -

//bool
//Player::isAdjustEnabled() const
//{
//  Q_ASSERT(isValid());
//  return ::libvlc_video_get_adjust_int(impl_->player(), libvlc_adjust_Enable);
//}
//
//void
//Player::setAdjustEnabled(bool t)
//{
//  Q_ASSERT(isValid());
//  int value = t ? 1 : 0;
//  ::libvlc_video_set_adjust_int(impl_->player(), libvlc_adjust_Enable, value);
//  emit adjustEnabledChanged(t);
//}

#define MAKE_ADJUST(_adjust, _Adjust, _type, _suffix) \
  _type \
  Player::_adjust() const \
  { \
    Q_ASSERT(isValid()); \
    return ::libvlc_video_get_adjust_##_suffix(impl_->player(), libvlc_adjust_##_Adjust); \
  } \
  void \
  Player::set##_Adjust(_type value) \
  { \
    Q_ASSERT(isValid()); \
    ::libvlc_video_set_adjust_int(impl_->player(), libvlc_adjust_Enable, 1); \
    ::libvlc_video_set_adjust_##_suffix(impl_->player(), libvlc_adjust_##_Adjust, value); \
    emit _adjust##Changed(_adjust()); \
  }

  MAKE_ADJUST(contrast, Contrast, qreal, float)
  MAKE_ADJUST(brightness, Brightness, qreal, float)
  MAKE_ADJUST(hue, Hue, int, int)
  MAKE_ADJUST(saturation, Saturation, qreal, float)
  MAKE_ADJUST(gamma, Gamma, qreal, float)

#undef MAKE_ADJUST

// EOF
// - PlayerListener -

/*
PlayerListener::PlayerListener(Player *player, QObject *parent)
  : Base(parent), player_(player),
    isPlayingChanged_(false),
    isPausedChaned_(false),
    isStoppedChaned_(false),
    mediaChanged_(false),
    timeChanged_(false),
    lengthChanged_(false),
    positionChanged_(false),
    volumeChanged_(false),
    encodingChanged_(false)
{
  Q_ASSERT(player_);
}

#define CONNECTED_SIGNAL(_signal) \
  bool \
  PlayerListener::connected_##_signal() const \
  { return _signal##_; }

  CONNECTED_SIGNAL(isPlayingChanged)
  CONNECTED_SIGNAL(isPausedChaned)
  CONNECTED_SIGNAL(isStoppedChaned)
  CONNECTED_SIGNAL(mediaChanged)
  CONNECTED_SIGNAL(timeChanged)
  CONNECTED_SIGNAL(lengthChanged)
  CONNECTED_SIGNAL(positionChanged)
  CONNECTED_SIGNAL(volumeChanged)
  CONNECTED_SIGNAL(encodingChanged)

#undef CONNECTED_SIGNAL

void
Player::setUpdateInterval(int milliseconds)
{
  Q_ASSERT(isValid());
  QTimer *timer = impl_->timer();
  Q_ASSERT(timer);
  if (timer->isActive())
    timer->stop();

  if (milliseconds > 0)
    timer->start(milliseconds);
}

int
Player::updateInterval() const
{
  Q_ASSERT(isValid());
  return impl_->timer()->isActive()? impl_->timer()->interval()
                                   : 0;
}
*/
