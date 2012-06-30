#ifndef PLAYER_P_H
#define PLAYER_P_H

#ifdef _MSC_VER
# pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
# pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER
#ifdef __clang__
# pragma clang diagnostic ignored "-Wunused-function" // vlc_plugin_path might be unused
#endif // __clang__

// player_p.h
// 11/26/2011

#include "module/player/player.h"
//#include "module/qtext/textcodec.h"
#include "module/qtext/countdowntimer.h"
#ifdef WITH_MODULE_VLCHTTP
# include "module/vlchttp/httpplugin.h"
#endif // WITH_MODULE_VLCHTTP
#include <QtCore/QObject>
#include <QtCore/QList>
#include <memory>
extern "C" {
# include <vlc/vlc.h>
} // extern "C"
#include <QtCore>

//#define VLC_DEBUG
#ifdef PLAYER_DEBUG
# define VLC_DEBUG
#endif // PLAYER_DEBUG

#ifdef Q_OS_MAC
# define unique_ptr  auto_ptr  // TR1 requires libc++
#endif // Q_OS_MAC

// - VLC arguments -
// http://wiki.videolan.org/VLC_command-line_help

#define APP_PLUGIN_PATH QCoreApplication::applicationDirPath() + "/plugins"
#ifdef Q_WS_WIN
# define APP_DATA_PATH QCoreApplication::applicationDirPath()
#else
# define APP_DATA_PATH QCoreApplication::applicationDirPath() + "/share"
#endif // Q_WS_WIN

#define _qs(cs) QString::fromAscii((cs))
#define _cs(qs) (qs).toAscii()    // toLocal8Bit is not recognized by VLC

namespace { // anonymous

  inline
  QByteArray vlcpath(const QString &path)
  {
#ifdef Q_WS_WIN
    return _cs(QString(path).replace("/", "\\"));
#else
    return _cs(path);
#endif // Q_WS_WIN
  }

  inline void vlc_reset_env()
  {
    static std::unique_ptr<char> auto_release_pool;
    QString qs = "VLC_PLUGIN_PATH=" + APP_PLUGIN_PATH;
    int n = qs.size() + 1;
    char *env = new char[n];
    ::strncpy(env, _cs(qs), n);
    ::putenv(env);
    auto_release_pool.reset(env);
  }

  inline const char *vlc_plugin_path()
  {
    static std::unique_ptr<char> auto_release_pool;
    char *ret = auto_release_pool.get();
    if (!ret) {
      QString path = vlcpath(APP_PLUGIN_PATH);
      int n = path.size() + 1;
      ret = new char[n];
      ::strncpy(ret, _cs(path), n); // use strncpy instead of strcpy in case sth goes wrong
      auto_release_pool.reset(ret);
    }
    return ret;
  }

  inline const char *vlc_data_path()
  {
    static std::unique_ptr<char> auto_release_pool;
    char *ret = auto_release_pool.get();
    if (!ret) {
      QString path = vlcpath(APP_DATA_PATH);
      int n = path.size() + 1;
      ret = new char[n];
      ::strncpy(ret, _cs(path), n); // use strncpy instead of strcpy in case sth goes wrong
      auto_release_pool.reset(ret);
    }
    return ret;
  }

} // anonymous namespace

#define VLC_ARGS_NULL           ""
#define VLC_PLUGIN_PATH         ::vlc_plugin_path()
#define VLC_DATA_PATH           ::vlc_data_path()
#define VLC_ARGS_CONFIG         "--ignore-config"       // Don't use VLC's config
#define VLC_ARGS_TITLE          "--no-video-title-show" // Don't display title
#define VLC_ARGS_LIBRARY        "--no-media-library"    // メディアライブラリーを使用 (デフォルトで有効)
#define VLC_ARGS_PLAYLIST       "--no-playlist-tree"    // プレイリストのツリー表示 (デフォルトで無効)
#define VLC_ARGS_INTERFACE      "-I", "dummy"           // Don't use any interface
#define VLC_ARGS_LOG            "--extraintf=logger"    // display log window
#define VLC_ARGS_PLUGIN_PATH    "--plugin-path", VLC_PLUGIN_PATH
#define VLC_ARGS_DATA_PATH      "--data-path", VLC_DATA_PATH
#define VLC_ARGS_VERBOSE(_lvl)  "--verbose=" #_lvl     // >= 0

// http://forums.techarena.in/windows-software/1403280.htm
//#define VLC_ARGS_MAC            "--vout=minimal_macosx", "--opengl-provider=minimal_macosx"
//#define VLC_ARGS_MAC            "--opengl-provider=minimal_macosx"
//#define VLC_ARGS_MAC            VLC_ARGS_NULL // use default macosx filter
#ifdef Q_WS_MAC
# define VLC_ARGS_OS           VLC_ARGS_DATA_PATH
#else
# define VLC_ARGS_OS           VLC_ARGS_NULL
#endif // Q_WS_MAC

#define VLC_ARGS_RELEASE \
  VLC_ARGS_CONFIG, \
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

// - PlayerPrivate bases -

namespace { // anonymous: vlc handle

  // See: libvlc_media_track_info_t
  struct TrackInfo
  {
    uint width, height; // Video
    uint channels, rate; // Audio
    uint videoCodecId, audioCodecId;

  public:
    TrackInfo()
      : width(0), height(0), channels(0), rate(0), videoCodecId(0), audioCodecId(0) { }

    bool isEmpty() const
    { return !(width || height || channels || rate || videoCodecId || audioCodecId); }

    void clear()
    { width = height = channels = rate = videoCodecId = audioCodecId = 0; }
  };

  class mp_handle_ {
    typedef QList<libvlc_media_t*> MediaList;

    libvlc_instance_t *instance_;     // library handle
    libvlc_media_player_t *player_;   // player handle
    //libvlc_media_list_player_t *list_player_; // playlist player handle
    libvlc_media_t *media_;           // media handle
    //libvlc_media_list_t *list_; // play list handle
    MediaList media_list_;

  public:
    libvlc_instance_t *instance() const { return instance_; }
    libvlc_media_player_t *player() const { return player_; }
    libvlc_media_t *media() const { return media_; }
    const MediaList &mediaList() const { return media_list_; }
    MediaList &mediaList() { return media_list_; }
    void setMedia(libvlc_media_t *media = 0) { media_ = media; }
    void setMediaList(const MediaList &l = MediaList()) { media_list_ = l; }

    bool valid() const { return instance_ && player_; }

  public:
    mp_handle_();
    ~mp_handle_();

    void reset();
  };

  inline
  mp_handle_::mp_handle_()
    : instance_(0), player_(0), media_(0)
  {
    //reset();
  }

  inline
  mp_handle_::~mp_handle_()
  {
    if (!media_list_.isEmpty())
      foreach (libvlc_media_t *m, media_list_)
        if (m  && m != media_)
          ::libvlc_media_release(m);
    if (media_)
      ::libvlc_media_release(media_);
#ifdef WITH_MODULE_VLCHTTP
    VlcHttpPlugin::unload();
#endif // WITH_MODULE_VLCHTTP
    if (player_) {
      ::libvlc_media_player_set_media(player_, 0);
      ::libvlc_media_player_release(player_);
    }
    if (instance_)
      ::libvlc_release(instance_);
  }

  inline void
  mp_handle_::reset()
  {
#ifdef Q_WS_MAC
    ::vlc_reset_env();
#endif // Q_WS_MAC

    if (!media_list_.isEmpty()) {
      foreach (libvlc_media_t *m, media_list_)
        if (m  && m != media_)
          ::libvlc_media_release(m);
      media_list_.clear();
    }
    if (media_) {
      ::libvlc_media_release(media_);
      media_ = 0;
    }

    if (player_) {
      ::libvlc_media_player_set_media(player_, 0);
      ::libvlc_media_player_release(player_);
    }
    if (instance_)
      ::libvlc_release(instance_);

    const char *vlc_argv[] = { VLC_ARGS };
    enum { vlc_argc = sizeof(vlc_argv)/sizeof(*vlc_argv) };
    instance_ = ::libvlc_new(vlc_argc, vlc_argv);
    Q_ASSERT(instance_);

#ifdef WITH_MODULE_VLCHTTP
    VlcHttpPlugin::load();
#endif // WITH_MODULE_VLCHTTP

    player_ = ::libvlc_media_player_new(instance_);
    Q_ASSERT(player_);

    //list_player_ = ::libvlc_media_list_player_new(instance_);
    //Q_ASSERT(list_player_);
    //::libvlc_media_list_player_set_media_player(list_player_, player_);
  }

} // anonymous namespace

namespace { // anonymous: player states

  // Track player states
  class mp_states_
  {
    bool paused_;
    bool embedded_;
    bool mouseEnabled_;
    bool keyEnabled_;
    bool mouseEventEnabled_;

    qint64 mediaSize_;
    QString mediaPath_;
    QString mediaTitle_;
    int subtitleId_;
    int titleId_;
    int trackNumber_;
    QStringList externalSubtitles_;

    QString userAgent_;

    QWidget *voutWindow_; // vout container

  public:
    mp_states_()
      : paused_(false), embedded_(false),
        mouseEnabled_(true), keyEnabled_(false), mouseEventEnabled_(false),
        mediaSize_(0),
        subtitleId_(0), titleId_(0), trackNumber_(0),
        voutWindow_(0)
    { }

    bool isPaused() const { return paused_; }
    void setPaused(bool t = true) { paused_ = t; }

    bool isEmbedded() const { return embedded_; }
    void setEmbedded(bool t = true) { embedded_ = t; }

    bool isKeyEnabled() const { return keyEnabled_; }
    void setKeyEnabled(bool t = true) { keyEnabled_ = t; }

    bool isMouseEnabled() const { return mouseEnabled_; }
    void setMouseEnabled(bool t = true) { mouseEnabled_ = t; }

    bool isMouseEventEnabled() const { return mouseEventEnabled_; }
    void setMouseEventEnabled(bool t = true) { mouseEventEnabled_ = t; }

    //bool isSubtitleVisible() const { return subtitleVisible_; }
    //void setSubtitleVisible(bool t = true) { subtitleVisible_ = t; }

    const qint64 &mediaSize() const { return mediaSize_; }
    void setMediaSize(qint64 size = 0) { mediaSize_ = size; }

    const QString &mediaPath() const { return mediaPath_; }
    void setMediaPath(const QString &path = QString()) { mediaPath_ = path; }

    const QString &mediaTitle() const { return mediaTitle_; }
    void setMediaTitle(const QString &title = QString()) { mediaTitle_ = title; }

    const QString &userAgent() const { return userAgent_; }
    void setUserAgent(const QString &agent = QString()) { userAgent_ = agent; }

    int trackNumber() const { return trackNumber_; }
    void setTrackNumber(int track = 0) { trackNumber_ = track; }

    int subtitleId() const { return subtitleId_; }
    void setSubtitleId(int id = 0) { subtitleId_ = id; }

    int titleId() const { return titleId_; }
    void setTitleId(int tid = 0) { titleId_ = tid; }

    /// Pathes of external subtitles
    QStringList &externalSubtitles() { return externalSubtitles_; }
    const QStringList &externalSubtitles() const { return externalSubtitles_; }
    void setExternalSubtitles(const QStringList &paths = QStringList()) { externalSubtitles_ = paths; }

    QWidget *voutWindow() const { return voutWindow_; }
    void setVoutWindow(QWidget *w = 0) { voutWindow_ = w; }
  };

  class mp_properties_
  {
    TrackInfo trackInfo_;
  public:
    TrackInfo &trackInfo() { return trackInfo_; }
    const TrackInfo &trackInfo() const { return trackInfo_; }
  };

  class mp_trackers_
  {
    QtExt::CountdownTimer *voutCountdown_;
  public:
    mp_trackers_() : voutCountdown_(0) { }

  public:
    QtExt::CountdownTimer *voutCountdown() const { return voutCountdown_; }
    void setVoutCountdown(QtExt::CountdownTimer *timer) { voutCountdown_ = timer; }
  };

  /*
  class mp_intl_
  {
    Core::TextCodec *codec_;

  public:
    mp_intl_() : codec_(0) { }

  public:
    Core::TextCodec *codec() const { return codec_; }
    void setCodec(Core::TextCodec *codec) { codec_ = codec; }
  };
  */

} // anonymous namespace

// - Slots -

/*
namespace Player_slots_ {

  class SetTrackNumber : public QObject
  {
    Q_OBJECT
    typedef QObject Base;

    Player *p_;
    int track_;
  public:
    SetTrackNumber(int track, Player *p)
      : Base(p), track_(track), p_(p)
    { Q_ASSERT(p_); }

  public slots:
    void trigger()
    {
      p_->setTrackNumber(track_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace Player_slots_
*/

#endif // PLAYER_P_H
