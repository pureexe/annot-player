// player.cc
// 6/30/2011

#ifdef _MSC_VER
  #pragma warning (disable: 4819)       // C4819: The file contains a character that cannot be represented in the current code page.
#endif // _MSC_VER

#include "player.h"
#include "core/util/codec.h"
#include <vlc/vlc.h>
#include <QtGui>
#include <cstring>
#ifdef Q_WS_MAC
  #include <QMacCocoaViewContainer>
#endif

#define DEBUG "Player"
#include "module/debug/debug.h"

//#define VLC_DEBUG
#ifdef PLAYER_DEBUG
  #define VLC_DEBUG
#endif // PLAYER_DEBUG

// - VLC core hacks -
// Enable core hacks by default.
#ifdef USE_VLCCORE
#ifndef MODULE_STRING
  #define MODULE_STRING "dummy"
#endif
#include <inttypes.h>
#include <vlc_vout.h>
#include <vlc_vout_display.h>

// Last checked: VLC 11.1, 7/20/2011
// See vlc/modules/video_output/wrapper.c
// See vlc/src/video_output/display.c
// See vlc/src/libvlc.c
// See vlc/src/control/core.c
// See vlc/src/control/media_player.c
//
// TODO: Study vlc_objects.h to guarantee type safety.

// Must consistent with wrapper.c
struct vout_sys_t {
    char           *title;
    vout_display_t *vd;
    bool           use_dr;
};

namespace { // anonymous
  namespace vlccore {

    // Must consistent with vout_display_owner_t::event
    typedef void (__cdecl *vout_display_event_handler)(vout_display_t*, int, va_list);
    void VoutDisplayEventMouse(vout_display_t *vd, int event, va_list args)
    {
      Q_ASSERT(0);
    }

    class vlc_object
    {
      typedef vlc_object Self;
      typedef vlc_object_t Impl;

      Impl *impl_;

    public:
      explicit vlc_object(void *object)
      { impl_ = VLC_OBJECT(object); Q_ASSERT(impl_); }

    public:
      vlc_object_t *self() const { return impl_; }
      vlc_object_t *parent() const { return impl_->p_parent; }

      static bool is_vlc_object(void *object)
      { return VLC_OBJECT(object); }
    };

    class libvlc_media_player : public vlc_object
    {
      typedef libvlc_media_player Self;
      typedef vlc_object Base;
      typedef libvlc_media_player_t Impl;

    public:
      explicit libvlc_media_player(void *object)
        : Base(object)
      {
        //std::fstream log("vlccore.log", std::fstream::out);
        //Q_ASSERT(log.good());
        //log << self()->psz_object_type<<std::endl;
        //log << parent()->psz_object_type<<std::endl;

        libvlc_int_t *libvlc = self()->p_libvlc;
        Q_ASSERT(libvlc);

        // TODO: need to figure out a way to find vout_display_t from libvlc_init_t

        // See media_player.c:lib_media_player_new:vlc_object_attach
        //libvlc_int_t *libvlc = reinterpret_cast<libvlc_int_t*>(parent());
        // libvlc_init is the same as libvlc_priv_t. See core.c for detail.
        //Q_ASSERT(libvlc);

        /*
        vout_thread_t *vout = reinterpret_cast<vout_thread_t*>(my_parent);
        Q_ASSERT(vout);
        vout_sys_t *sys = vout->p_sys;
        log << sys->title << std::endl;
        vout_display_t *vd = sys->vd;
        Q_ASSERT(vd);
        log << vd ->psz_object_type << std::endl;
        vout_display_event_handler handler = vd->owner.event;
        vd->owner.event = VoutDisplayEventMouse;
        */
      }
    };
  } // namespace vlccore

} // anonymous namespace
#endif // USE_VLCCORE

// - VLC extensions -
// http://wiki.videolan.org/VLC_command-line_help
#define VLC_PLUGIN_PATH         (QCoreApplication::applicationDirPath() + "/plugins").toAscii()

#define VLC_ARGS_NULL           ""
#define VLC_ARGS_CONFIG         "--ignore-config"       // Don't use VLC's config
#define VLC_ARGS_TITLE          "--no-video-title-show" // Don't display title
#define VLC_ARGS_LIBRARY        "--no-media-library"    // メディアライブラリーを使用 (デフォルトで有効)
#define VLC_ARGS_PLAYLIST       "--no-playlist-tree"    // プレイリストのツリー表示 (デフォルトで無効)
#define VLC_ARGS_INTERFACE      "-I", "dummy"           // Don't use any interface
#define VLC_ARGS_LOG            "--extraintf=logger"    // display log window
#define VLC_ARGS_PLUGIN_PATH    "--plugin-path", VLC_PLUGIN_PATH
#define VLC_ARGS_VERBOSE(_lvl)  "--verbose=" #_lvl     // >= 0

#define VLC_ARGS_MAC            "--vout=minimal_macosx", "--opengl-provider=minimal_macosx"
                                // http://forums.techarena.in/windows-software/1403280.htm
#ifdef Q_WS_MAC
  #define VLC_ARGS_OS           VLC_ARGS_MAC
#else
  #define VLC_ARGS_OS           VLC_ARGS_NULL
#endif // Q_WS_MAC

#define VLC_ARGS_RELEASE \
  VLC_ARGS_CONFIG, \
  VLC_ARGS_PLUGIN_PATH, \
  VLC_ARGS_INTERFACE, \
  VLC_ARGS_TITLE, \
  VLC_ARGS_LIBRARY, \
  VLC_ARGS_PLAYLIST, \
  VLC_ARGS_OS
#define VLC_ARGS_DEBUG \
  VLC_ARGS_RELEASE, \
  VLC_ARGS_LOG, \
  VLC_ARGS_VERBOSE(2)

// TODO: make this an option
#ifdef VLC_DEBUG
  #define VLC_ARGS                VLC_ARGS_DEBUG
#else
  #define VLC_ARGS                VLC_ARGS_RELEASE
#endif

#define VLC_MAX_VOLUME 100.0

// http://mailman.videolan.org/pipermail/vlc-devel/2010-April/074742.html
// Adapt container window type
#if defined(Q_OS_WIN)
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_hwnd(_mp, _wid)
#elif defined(Q_OS_MAC)
  //#define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_agl(_mp, _wid)
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_nsobject(_mp, _wid)
#else //Linux
  #define libvlc_media_player_set_drawable(_mp, _wid)   libvlc_media_player_set_xwindow(_mp, _wid)
#endif

#if defined(Q_OS_WIN)
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_hwnd(_mp)
#elif defined(Q_OS_MAC)
  //#define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_agl(_mp)
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_nsobject(_mp)
#else //Linux
  #define libvlc_media_player_get_drawable(_mp)         libvlc_media_player_get_xwindow(_mp)
#endif

// - PlayerImpl -

namespace { // anonymous: vlc handle

  class vlc_handle {
    libvlc_instance_t *instance_;     // library handle
    libvlc_media_player_t *player_;   // player handle
    libvlc_media_t *media_;           // media handle

  public:
    libvlc_instance_t *instance() const { return instance_; }
    libvlc_media_player_t *player() const { return player_; }
    libvlc_media_t *media() const { return media_; }

    void setMedia(libvlc_media_t *media = 0) { media_ = media; }

    bool valid() const { return instance_ && player_; }

  public:
    vlc_handle();
    ~vlc_handle();
  };

  vlc_handle::vlc_handle()
  {
    const char *vlc_argv[] = { VLC_ARGS };
    int vlc_argc = sizeof(vlc_argv)/sizeof(*vlc_argv);
    instance_ = libvlc_new(vlc_argc, vlc_argv);
    Q_ASSERT(instance_);

    player_ = libvlc_media_player_new(instance_);
    Q_ASSERT(player_);

#ifdef USE_VLCCORE
    vlccore::libvlc_media_player((void*)player_);
#endif // USE_VLCCORE
  }

  vlc_handle::~vlc_handle()
  {
    // The player would be automatically released when the instance is released
    // Otherwise there will be a double-free segmentation fault.
    //if (player_)
    //  libvlc_media_player_release(player_);
    if (instance_)
      libvlc_release(instance_);
  }

} // anonymous namespace

namespace { // anonymous: player states

  // Track player states
  class mp_states
  {
    bool paused_;
    bool embedded_;
    bool mouseEnabled_;
    bool keyEnabled_;

    QString mediaPath_;
    int subtitleId_;
    QStringList externalSubtitles_;

  public:
    mp_states()
      : paused_(false), embedded_(false), mouseEnabled_(true), keyEnabled_(false), subtitleId_(0) { }

    bool isPaused() const { return paused_; }
    void setPaused(bool t = true) { paused_ = t; }

    bool isEmbedded() const { return embedded_; }
    void setEmbedded(bool t = true) { embedded_ = t; }

    bool isKeyEnabled() const { return keyEnabled_; }
    void setKeyEnabled(bool t = true) { keyEnabled_ = t; }

    bool isMouseEnabled() const { return mouseEnabled_; }
    void setMouseEnabled(bool t = true) { mouseEnabled_ = t; }

    //bool isSubtitleVisible() const { return subtitleVisible_; }
    //void setSubtitleVisible(bool t = true) { subtitleVisible_ = t; }

    const QString &mediaPath() const { return mediaPath_; }
    void setMediaPath(const QString &path = QString()) { mediaPath_ = path; }

    int subtitleId() const { return subtitleId_; }
    void setSubtitleId(int id = 0) { subtitleId_ = id; }

    /// Pathes of external subtitles
    QStringList &externalSubtitles() { return externalSubtitles_; }
    const QStringList &externalSubtitles() const { return externalSubtitles_; }
    void setExternalSubtitles(const QStringList &paths = QStringList()) { externalSubtitles_ = paths; }
  };

  class mp_tracker
  {
    QTimer *timer_;
  public:
    mp_tracker() : timer_(0) { }

  public:
    QTimer *timer() const { return timer_; }
    void setTimer(QTimer *timer) { timer_ = timer; }
  };

  class mp_intl
  {
    Core::Codec *codec_;

  public:
    mp_intl() : codec_(0) { }

  public:
    Core::Codec *codec() const { return codec_; }
    void setCodec(Core::Codec *codec) { codec_ = codec; }
  };

} // anonymous namespace

class PlayerImpl
  : public vlc_handle,
    public mp_states,
    //public mp_tracker,
    public mp_intl
{ };

// - Player -

// ++ Event handlers ++
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
  namespace vlc_event_handler {
#define VLC_EVENT_HANDLER(_signal) \
    void _signal(const struct libvlc_event_t *event, void *media_player_instance) \
    { \
      DOUT("signal: " #_signal ": enter"); \
      Q_UNUSED(event); \
      Player *p = static_cast<Player*>(media_player_instance); \
      Q_ASSERT(p); \
      p->emit_##_signal(); \
      DOUT("signal: " #_signal ": exit"); \
    }

    VLC_EVENT_HANDLER(opening)
    VLC_EVENT_HANDLER(buffering)
    VLC_EVENT_HANDLER(playing)
    VLC_EVENT_HANDLER(stopped)
    VLC_EVENT_HANDLER(paused)
    VLC_EVENT_HANDLER(timeChanged)
    VLC_EVENT_HANDLER(lengthChanged)
    VLC_EVENT_HANDLER(positionChanged)
    VLC_EVENT_HANDLER(mediaChanged)
    VLC_EVENT_HANDLER(errorEncountered)
#undef MEDIA_PLAYER_EVENT_HANDLER
  }
} // anonymous namespace

// ++ Static settings ++

QStringList
Player::supportedVideoSuffices()
{ return QStringList() PLAYER_FORMAT_VIDEO(<<); }

QStringList
Player::supportedAudioSuffices()
{ return QStringList() PLAYER_FORMAT_AUDIO(<<); }

QStringList
Player::supportedSubtitleSuffices()
{ return QStringList() PLAYER_FORMAT_SUBTITLE(<<); }

QStringList
Player::supportedVideoFilters()
{ return QStringList() PLAYER_FORMAT_VIDEO(<< "*."); }

QStringList
Player::supportedAudioFilters()
{ return QStringList() PLAYER_FORMAT_AUDIO(<< "*."); }

QStringList
Player::supportedSubtitleFilters()
{ return QStringList() PLAYER_FORMAT_SUBTITLE(<< "*."); }

// ++ Constructions ++

Player::Player(QObject *parent)
  : Base(parent), impl_(0)
{
  DOUT("constructor: enter");
  reset();
  DOUT("constructor: exit");
}

Player::~Player()
{
  DOUT("destructor: enter");
  if (impl_)
    delete impl_;
  DOUT("destructor: exit");
}

bool
Player::isValid() const
{ return impl_ && impl_->valid(); }


void
Player::reset()
{
  DOUT("reset: enter");
  if (impl_)
    delete impl_;

  impl_ = new Impl;
  Q_ASSERT(isValid());

  // Set event handlers.
  libvlc_event_manager_t *event_manager = libvlc_media_player_event_manager(impl_->player());
  Q_ASSERT(event_manager);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerOpening, vlc_event_handler::opening, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerOpening, vlc_event_handler::buffering, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerPlaying, vlc_event_handler::playing, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerPaused, vlc_event_handler::paused, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerStopped, vlc_event_handler::stopped, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerTimeChanged, vlc_event_handler::timeChanged, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerLengthChanged, vlc_event_handler::lengthChanged, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerPositionChanged, vlc_event_handler::positionChanged, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerMediaChanged, vlc_event_handler::mediaChanged, this);
  libvlc_event_attach(event_manager, libvlc_MediaPlayerEncounteredError, vlc_event_handler::errorEncountered, this);

  // Set timer.
  //impl_->setTimer(new QTimer(this));
  //connect(impl_->timer(), SIGNAL(timeout()), SLOT(update()));
  //setUpdateInterval();

  DOUT("reset: exit");
}

// ++ Embedding ++

void
Player::setEmbeddedWindowHandle(WindowHandle handle)
{
  Q_ASSERT(isValid());
  Q_ASSERT(handle);
  if (handle) {
    impl_->setEmbedded();
    libvlc_media_player_set_drawable(impl_->player(), handle);
  }
}

Player::WindowHandle
Player::embeddedWindowHandle() const
{
  Q_ASSERT(isValid());
  return static_cast<WindowHandle>(
        libvlc_media_player_get_drawable(impl_->player())
        );
}

#ifdef Q_WS_MAC
void
Player::embed(QMacCocoaViewContainer *w)
{
  DOUT("embed: enter");
  Q_ASSERT(isValid());
  Q_ASSERT(w);
  if (w)
    setEmbeddedWindowHandle(w->cocoaView());
  DOUT("embed: exit");
}
#else
void
Player::embed(QWidget *w)
{
  DOUT("embed: enter");
  Q_ASSERT(isValid());
  Q_ASSERT(w);
  if (w)
    setEmbeddedWindowHandle(w->winId());
  DOUT("embed: exit");
}
#endif // Q_WS_MAC

bool
Player::isEmbedded() const
{
  Q_ASSERT (isValid());
  return impl_->isEmbedded();
}

// ++ Encoding ++
QString
Player::encoding() const
{
  Q_ASSERT(isValid());
  return impl_->codec()->encoding();
}

void
Player::setEncoding(const QString &encoding)
{
  Q_ASSERT(isValid());

  if (impl_->codec())
    delete impl_->codec();
  impl_->setCodec(new Core::Codec(encoding, this));
  emit encodingChanged();
}


QByteArray
Player::decode(const QString &input) const
{
  return impl_ && impl_->codec() ?
        impl_->codec()->encode(input)
      : input.toAscii();
}

QString
Player::encode(const char *input) const
{
  return impl_ && impl_->codec() ?
        impl_->codec()->decode(input)
      : QString(input);
}

// ++ Playing states ++
bool
Player::isPlaying() const
{
  Q_ASSERT(isValid());
  return libvlc_media_player_is_playing(impl_->player());
}

bool
Player::isPaused() const
{
  Q_ASSERT(isValid());
  return impl_->isPaused();
}

bool
Player::isStopped() const
{ return !isPlaying() && !isPaused(); }

Player::Status
Player::status() const
{
  if (isPlaying())
    return Playing;
  else if (isPaused())
    return Paused;
  else
    return Stopped;
}

bool
Player::pausable() const
{ return hasMedia() && libvlc_media_player_can_pause(impl_->player()); }

bool
Player::seekable() const
{ return hasMedia() && libvlc_media_player_is_seekable(impl_->player()); }

// ++ Open media ++

void
Player::clearMedia()
{
  DOUT("clearMedia: enter");
  if (isValid()) {
    if (hasMedia())
      closeMedia();
  }
  DOUT("clearMedia: exit");
}

bool
Player::openMedia(const QString &path)
{
  DOUT("openMedia: enter:" << path);
  Q_ASSERT(isValid());
  if (hasMedia())
    closeMedia();

  DOUT("openMedia: open:" << decode(path));
  impl_->setMedia(
        //libvlc_media_new_path(impl_->instance(), path)   // local file only
        libvlc_media_new_location(impl_->instance(), decode(path)) // URL
        );

  if (!impl_->media()) {
    impl_->setMediaPath();
    DOUT("WARNING: openMedia: failed");
    return false;
  }

  libvlc_media_player_set_media(impl_->player(), impl_->media());

  impl_->setMediaPath(path);
  DOUT("openMedia: exit");
  return true;
}

void
Player::closeMedia()
{
  DOUT("openMedia: enter");
  Q_ASSERT(isValid());

  impl_->setPaused(false);
  impl_->setSubtitleId();
  impl_->setMediaPath();
  impl_->setMedia();
  impl_->setExternalSubtitles();

  libvlc_media_player_set_media(impl_->player(), 0);

  emit mediaClosed();
  DOUT("closeMedia: exit");
}

bool
Player::hasMedia() const
{
  Q_ASSERT(isValid());
  return impl_->media()
      && libvlc_media_player_get_media(impl_->player());
}

// ++ Full screen ++
void
Player::setFullScreen(bool t)
{
  Q_ASSERT(isValid());
  libvlc_set_fullscreen(impl_->player(), t);
}

bool
Player::isFullScreen() const
{
  Q_ASSERT(isValid());
  return libvlc_get_fullscreen(impl_->player());
}

void
Player::toggleFullScreen()
{
  Q_ASSERT(isValid());
  libvlc_toggle_fullscreen(impl_->player());
}

// ++ Media information ++

QString
Player::mediaTitle() const
{
  Q_ASSERT(isValid());
  if (!hasMedia())
    return QString();

  const char *title = libvlc_media_get_meta(impl_->media(), libvlc_meta_Title);
  // VLC i18n bug. It simpily cannot handle UTF-8 correctly.
  if (!title || ::strstr(title, "??"))
    return QString();
  else
    return encode(title);
}

QString
Player::mediaPath() const
{
  Q_ASSERT(isValid());
  return impl_->mediaPath();
}

// ++ Play control ++

void
Player::play()
{
  DOUT("play: enter");
  if (hasMedia()) {
    impl_->setPaused(false);
    libvlc_media_player_play(impl_->player());
  }
  DOUT("play: exit");
}

void
Player::stop()
{
  DOUT("stop: enter");
  if (hasMedia()) {
    impl_->setPaused(false);
    libvlc_media_player_stop(impl_->player());
  }
  DOUT("stop: exit");
}

void
Player::pause()
{
  DOUT("pause: enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(true);
    libvlc_media_player_set_pause(impl_->player(), true);
  }
  DOUT("pause: exit");
}

void
Player::resume()
{
  DOUT("resume: enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(false);
    libvlc_media_player_set_pause(impl_->player(), false);
  }
  DOUT("resume: exit");
}

void
Player::playPause()
{
  DOUT("playPause: enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused(!impl_->isPaused());
    libvlc_media_player_pause(impl_->player());
  }
  DOUT("playPause: exit");
}

void
Player::nextFrame()
{
  DOUT("nextFrame: enter");
  if (hasMedia() && pausable()) {
    impl_->setPaused();
    libvlc_media_player_next_frame(impl_->player());
  }
  DOUT("nextFrame: exit");
}

bool
Player::snapshot(const QString &path)
{
  DOUT("snapshot: enter");
  if (!hasMedia()) {
    DOUT("snapshot: no media, ignored");
    return false;
  }

  QByteArray path_cstr = decode(path);
  DOUT("nextFrame: exit");
  return !libvlc_video_take_snapshot(impl_->player(), 0, path_cstr, 0, 0);
}


// ++ Set/get properties ++

void
Player::setMouseEnabled(bool enable)
{
  Q_ASSERT(isValid());
  impl_->setMouseEnabled(enable);
  libvlc_video_set_mouse_input(impl_->player(), enable);
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
  libvlc_video_set_key_input(impl_->player(), enable);
}

bool
Player::isKeyboardEnabled() const
{
  Q_ASSERT(isValid());
  return impl_->isKeyEnabled();
}

void
Player::setVolume(qreal vol)
{
  DOUT("setVolume: enter");
  Q_ASSERT(isValid());
  libvlc_audio_set_volume(impl_->player(),
                          static_cast<int>(vol * VLC_MAX_VOLUME));
  emit volumeChanged();
  DOUT("setVolume: exit");
}

void
Player::setPosition(qreal pos)
{
  DOUT("setPosition: enter");
  if (hasMedia())
    libvlc_media_player_set_position(impl_->player(), pos);
  DOUT("setPosition: exit");
}


qreal
Player::volume() const
{
  Q_ASSERT(isValid());
  return libvlc_audio_get_volume(impl_->player()) / VLC_MAX_VOLUME;
}

qreal
Player::position() const
{ return hasMedia()? libvlc_media_player_get_position(impl_->player()) : 0; }

// - Play time -
qint64
Player::mediaLength() const
{
  Q_ASSERT(isValid());
  return libvlc_media_player_get_length(impl_->player());
}

qint64
Player::time() const
{
  Q_ASSERT(isValid());
  return libvlc_media_player_get_time(impl_->player());
}

void
Player::setTime(qint64 time)
{
  DOUT("setTime: enter");
  Q_ASSERT(isValid());
  if (hasMedia())
    libvlc_media_player_set_time(impl_->player(), time);
  DOUT("setTime: exit");
}

// - Subtitles -

int
Player::subtitleCount() const
{
  Q_ASSERT(isValid());
  return ::libvlc_video_get_spu_count(impl_->player());
}

bool
Player::hasSubtitles() const
{
  Q_ASSERT(isValid());
  return ::libvlc_video_get_spu_description(impl_->player());
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
    if (first && first->psz_name)
      ret.append(encode(first->psz_name));
    first = first->p_next;
  }

  return ret;
}

int
Player::subtitleId() const
{
  Q_ASSERT(isValid());
  int id = ::libvlc_video_get_spu(impl_->player());
  if (id > 0)
    return id - 1;
  else
    return impl_->subtitleId() - 1;
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
    DOUT("setSubtitleId: failed: id =" << id);
}

bool
Player::openSubtitle(const QString &fileName)
{ return setSubtitleFromFile(fileName); }

bool
Player::setSubtitleFromFile(const QString &fileName)
{
  DOUT("setSubtitleFromFile: enter:" << fileName);
  Q_ASSERT(isValid());

  QString path = fileName;
#ifdef Q_WS_WIN
  path.replace('/', '\\');
#endif // Q_WS_WIN

  DOUT("opening subtitle:" << decode(path));
  if (impl_->externalSubtitles().indexOf(path) >= 0) {
    DOUT("subtitle already loaded");
    DOUT("setSubtitleFromFile: exit");
    return true;
  }

  bool ok = ::libvlc_video_set_subtitle_file(impl_->player(), decode(path));
  if (ok) {
    DOUT("succeeded, number of subtitles (0 for the first time):" << ::libvlc_video_get_spu_count(impl_->player()));
    impl_->externalSubtitles() << path;
    impl_->setSubtitleId(::libvlc_video_get_spu_count(impl_->player()) - 1);
    emit subtitleChanged();
  } else
    DOUT("setSubtitleFromFile: failed");

  DOUT("setSubtitleFromFile: exit");
  return ok;
}

void
Player::addSubtitleFromFile(const QString &fileName)
{
  Q_ASSERT(isValid());
  int bak = ::libvlc_video_get_spu(impl_->player());
  if (bak > 0)
    impl_->setSubtitleId(bak);
  ::libvlc_video_set_subtitle_file(impl_->player(), decode(fileName));
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
  DOUT("searchExternalSubtitles: enter");
  QStringList ret;
  if (hasMedia()) {
    QString fileName = mediaPath();
    QString baseName =QFileInfo(fileName).baseName() ;
    QDir dir = QFileInfo(fileName).absolutePath();
    DOUT("search in directory:" << dir.path());
    if (dir.exists()) {
      QStringList filters = supportedSubtitleFilters();
      foreach (const QString fileName, dir.entryList(filters, QDir::Files))
        if (fileName.startsWith(baseName + "."))
          ret << dir.path() + "/" + fileName;
    } else
      DOUT("directory to search not existed");
  }

  DOUT("found subtitles count:" << ret.size());
  DOUT("searchExternalSubtitles: exit");
  return ret;
}

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
*/

/*
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

// EOF
