#ifndef PLAYER_H
#define PLAYER_H

// player.h
// 6/30/2011

#include "player_config.h"
#include <QWidget> // where WId is declared
#include <QStringList>

#ifdef Q_WS_MAC
QT_FORWARD_DECLARE_CLASS(QMacCocoaViewContainer)
#endif // Q_WS_MAC

class PlayerImpl; ///< \internal
// Player
/**
 *  \brief  Media player controller
 *
 *  Currently, multiple documents is not allowed.
 */
class Player : public QObject
{
  Q_OBJECT
  typedef Player Self;
  typedef QObject Base;
  typedef PlayerImpl Impl;

  Impl *impl_;

  // - Properties -
public:
  static QStringList supportedAudioSuffices();
  static QStringList supportedVideoSuffices();
  static QStringList supportedSubtitleSuffices();

  static QStringList supportedAudioFilters();
  static QStringList supportedVideoFilters();
  static QStringList supportedSubtitleFilters();

  // - Constructions -
private:
  Player(const Self&); ///< Copy not allowed.

public:
  explicit Player(QObject *parent = 0);
  ~Player();

  bool isValid() const;

  QString encoding() const;

  // - Queries -
public:
  bool isEmbedded() const;
  bool isFullScreen() const;
  bool hasMedia() const;
  bool isMouseEnabled() const;
  bool isKeyboardEnabled() const;

  enum Status { Stopped = 0, Playing = 1, Paused = 2 };
  Status status() const;
  bool isPlaying() const;
  bool isPaused() const;
  bool isStopped() const;
  bool pausable() const;      // some media cannot be paused
  bool seekable() const;      // some media cannot be paused

  QString mediaTitle() const;
  QString mediaPath() const;

  /**
   *  \brief  Total play time in msecs.
   *
   *  When no media OR THE MEDIA WAS JUST OPENED, return -1.
   *  Due to deficiency of VLC, there is a delay between mediaChanged and lengthChanged signals.
   */
  qint64 mediaLength() const;

  ///  Current played time in msecs, always in [0, mediaLength()].
  qint64 time() const;

public:
#ifdef Q_WS_MAC
  typedef void *WindowHandle; // NSObject
#else
  typedef WId WindowHandle;
#endif // Q_WS_MAC
  void setEmbeddedWindowHandle(WindowHandle wid);
  WindowHandle embeddedWindowHandle() const;

  ///  A shortcut of setWinId for QWidget.
#ifdef Q_WS_MAC
  void embed(QMacCocoaViewContainer *w);
#else
  void embed(QWidget *w);
#endif // Q_WS_MAC

  ///  Save the current frame as picture. Return if succeed.
  bool snapshot(const QString &savePath);

  int subtitleCount() const;    ///< Number of subtitles
  int subtitleId() const;       ///< Return current used subtitle
  bool hasSubtitles() const;
  QStringList subtitleDescriptions() const; ///< Return list of internal/external subtitles
  bool isSubtitleVisible() const;
  void addSubtitleFromFile(const QString &fileName);
  bool setSubtitleFromFile(const QString &fileName);
  QStringList searchExternalSubtitles() const; ///< return external subtitle files

  // - Slots -
public slots:

  ///  All settings except parent and connections would be lost. Should be used for error recovery only.
  void reset();

  ///  Restore status before opening media. Should be used for error recovery only.
  void clearMedia();

  /**
   *  Play the specified file.
   *  If hasMedia, the previous media will be automatically released.
   *
   *  \p filePath format:
   *  - [file://]filename              Plain media file
   *  - http://ip:port/file            HTTP URL
   *  - ftp://ip:port/file             FTP URL
   *  - mms://ip:port/file             MMS URL
   *  - screen://                      Screen capture
   *  - [dvd://][device][@raw_device]  DVD device
   *  - [vcd://][device]               VCD device
   *  - [cdda://][device]              Audio CD device
   *  - udp:[[<source address>]@[<bind address>][:<bind port>]]
   */
  bool openMedia(const QString &url);
  void closeMedia();

  void setEncoding(const QString &encoding); ///< See \c Encoding for details.

  bool openSubtitle(const QString &fileName); ///< Same as \c setSubtitleFromFile

  ///  Only work if embed/setWinId is not invoked.
  void setFullScreen(bool t = true);
  void toggleFullScreen();

  void setMouseEnabled(bool enabled = true);
  void setKeyboardEnabled(bool enabled = true);

  void play();
  void stop();
  void pause();
  void resume();
  void playPause(); ///< Toggle play/pause.
  void nextFrame(); ///< Pause and jump to the next frame.

  qreal volume() const;       ///< [0,1]
  qreal position() const;     ///< [0,1]
  void setVolume(qreal vol);
  void setPosition(qreal pos);

  void setTime(qint64 msecs); ///< same effect as setPosition

  void setSubtitleId(int id);   ///< id of available subtitles
  void showSubtitle();
  void hideSubtitle();
  void setSubtitleVisible(bool visible);

  void loadExternalSubtitles(); ///< try to automatically load external subtitles

  // - Signals -
signals:
  void opening();
  void buffering();
  void playing();
  void paused();
  void stopped();
  void mediaChanged();  // Actually mediaAdded
  void mediaClosed();
  void timeChanged();
  void lengthChanged(); // Due to deficency of VLC, the length is changed after mediaChangd.
  void positionChanged();
  void volumeChanged();
  void encodingChanged();
  void errorEncountered();
  void subtitleChanged();

  // FIXME: This is not a good style to expose emit outside.
  // It could be better to use event to trigger emittion!!!!!!!!!!!!!!!!1
  // TODO: add a controlevent, and use it to communicate
  // Override event() to achieve this..
  //@{
  // \internal
public:
#define ADD_SIGNAL(_signal)    void emit_##_signal { emit _signal; }
    ADD_SIGNAL(opening())
    ADD_SIGNAL(buffering())
    ADD_SIGNAL(playing())
    ADD_SIGNAL(paused())
    ADD_SIGNAL(stopped())
    ADD_SIGNAL(mediaChanged())
    ADD_SIGNAL(timeChanged())
    ADD_SIGNAL(lengthChanged())
    ADD_SIGNAL(positionChanged())
    ADD_SIGNAL(volumeChanged())
    ADD_SIGNAL(encodingChanged())
    ADD_SIGNAL(errorEncountered())
#undef ADD_SIGNAL
  //@}

  // - Implementations -
protected:
  QByteArray decode(const QString &utf) const;
  QString encode(const char *ascii) const;
};


/*
class PlayerListener : public QObject
{
  Q_OBJECT
  typedef PlayerListener Self;
  typedef QObject Base;

  Player *player_;

public:
  explicit PlayerListener(Player *player, QObject *parent = 0);

#define ADD_SIGNAL(_signal) \
  signals: \
    void _signal(bool); \
  public slots: \
    void connect_##_signal(); \
    void disconnect_##_signal(); \
  public: \
    bool connected_##_signal() const; \
  private: \
    bool _signal##_;

  ADD_SIGNAL(isPlayingChanged)
  ADD_SIGNAL(isPausedChaned)
  ADD_SIGNAL(isStoppedChaned)
  ADD_SIGNAL(mediaChanged)
  ADD_SIGNAL(timeChanged)
  ADD_SIGNAL(lengthChanged)
  ADD_SIGNAL(positionChanged)
  ADD_SIGNAL(volumeChanged)
  ADD_SIGNAL(encodingChanged)
#undef ADD_SIGNAL
}
*/

#endif // PLAYER_H
