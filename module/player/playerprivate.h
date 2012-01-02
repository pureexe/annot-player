#ifndef PLAYERPRIVATE_H
#define PLAYERPRIVATE_H

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

// playerprivate.h
// 11/26/2011

#include "player.h"
#include "core/util/codec.h"
#include "core/util/countdowntimer.h"
#include <QObject>
#include <QList>
#include <memory>
extern "C" {
  #include <vlc/vlc.h>
} // extern "C"
#include <QtCore>

//#define VLC_DEBUG
#ifdef PLAYER_DEBUG
  #define VLC_DEBUG
#endif // PLAYER_DEBUG

// - VLC arguments -
// http://wiki.videolan.org/VLC_command-line_help

namespace { // anonymous

  // Persistent storage
  const char *vlc_plugin_path()
  {
    static std::auto_ptr<char> auto_release_pool;
    char *ret = auto_release_pool.get();
    if (!ret) {
      QString path = QCoreApplication::applicationDirPath() + "/plugins";
      const char *src = path.toAscii();
      int n = ::strlen(src);
      Q_ASSERT(n);
      if (n) {
        ret = new char[n + 1];
        ::strncpy(ret, src, n + 1); // use strncpy instead of strcpy in case sth goes wrong
        auto_release_pool.reset(ret);
      }
    }
    return ret;
  }
} // anonymous namespace

#define VLC_ARGS_NULL           ""
#define VLC_PLUGIN_PATH         ::vlc_plugin_path()
#define VLC_ARGS_CONFIG         "--ignore-config"       // Don't use VLC's config
#define VLC_ARGS_TITLE          "--no-video-title-show" // Don't display title
#define VLC_ARGS_LIBRARY        "--no-media-library"    // メディアライブラリーを使用 (デフォルトで有効)
#define VLC_ARGS_PLAYLIST       "--no-playlist-tree"    // プレイリストのツリー表示 (デフォルトで無効)
#define VLC_ARGS_INTERFACE      "-I", "dummy"           // Don't use any interface
#define VLC_ARGS_LOG            "--extraintf=logger"    // display log window
#define VLC_ARGS_PLUGIN_PATH    "--plugin-path", VLC_PLUGIN_PATH
#define VLC_ARGS_VERBOSE(_lvl)  "--verbose=" #_lvl     // >= 0

// http://forums.techarena.in/windows-software/1403280.htm
//#define VLC_ARGS_MAC            "--vout=minimal_macosx", "--opengl-provider=minimal_macosx"
#define VLC_ARGS_MAC            "--opengl-provider=minimal_macosx"
//#define VLC_ARGS_MAC            VLC_ARGS_NULL // use default macosx filter
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

// - PlayerImpl bases -

namespace { // anonymous: vlc handle

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

  mp_handle_::mp_handle_()
    : instance_(0), player_(0), media_(0)
  {
    //reset();
  }

  mp_handle_::~mp_handle_()
  {
    // The player would be automatically released when the instance is released
    // Otherwise there will be a double-free segmentation fault.
    if (instance_)
      ::libvlc_release(instance_);
  }

  void
  mp_handle_::reset()
  {
    if (instance_)
      ::libvlc_release(instance_);

    const char *vlc_argv[] = { VLC_ARGS };
    int vlc_argc = sizeof(vlc_argv)/sizeof(*vlc_argv);
    instance_ = ::libvlc_new(vlc_argc, vlc_argv);
    Q_ASSERT(instance_);

    player_ = ::libvlc_media_player_new(instance_);
    Q_ASSERT(player_);

    media_ = 0;

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

    QString mediaPath_;
    int subtitleId_;
    int titleId_;
    int trackNumber_;
    QStringList externalSubtitles_;

    QWidget *voutWindow_; // vout container

  public:
    mp_states_()
      : paused_(false), embedded_(false),
        mouseEnabled_(true), keyEnabled_(false), mouseEventEnabled_(false),
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

    const QString &mediaPath() const { return mediaPath_; }
    void setMediaPath(const QString &path = QString()) { mediaPath_ = path; }

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

  class mp_trackers_
  {
    Core::CountdownTimer *voutCountdown_;
  public:
    mp_trackers_() : voutCountdown_(0) { }

  public:
    Core::CountdownTimer *voutCountdown() const { return voutCountdown_; }
    void setVoutCountdown(Core::CountdownTimer *timer) { voutCountdown_ = timer; }
  };

  class mp_intl_
  {
    Core::Codec *codec_;

  public:
    mp_intl_() : codec_(0) { }

  public:
    Core::Codec *codec() const { return codec_; }
    void setCodec(Core::Codec *codec) { codec_ = codec; }
  };

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
    void setTrackNumber()
    {
      p_->setTrackNumber(track_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace Player_slots_
*/

#endif // PLAYERPRIVATE_H
