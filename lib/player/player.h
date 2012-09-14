#ifndef PLAYER_H
#define PLAYER_H

// player.h
// 6/30/2011

#include <QtCore/QStringList>
#include <QtGui/QWidget> // where WId is declared

QT_BEGIN_NAMESPACE
class QMacCocoaViewContainer;
class QNetworkCookieJar;
QT_END_NAMESPACE

class PlayerPrivate; ///< \internal
struct libvlc_media_t;


class Player : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Player)
  typedef Player Self;
  typedef QObject Base;
  typedef PlayerPrivate Private;

  Private *d_;

  // - Properties -
public:
  static const QStringList &supportedAudioSuffices();
  static const QStringList &supportedVideoSuffices();
  static const QStringList &supportedPictureSuffices();
  static const QStringList &supportedSubtitleSuffices();
  static const QStringList &supportedPlaylistSuffices();
  static const QStringList &supportedImageSuffices();
  static const QStringList &supportedSuffices();

  static const QStringList &supportedAudioFilters();
  static const QStringList &supportedVideoFilters();
  static const QStringList &supportedPictureFilters();
  static const QStringList &supportedSubtitleFilters();
  static const QStringList &supportedPlaylistFilters();
  static const QStringList &supportedImageFilters();
  static const QStringList &supportedFilters();

  static bool isSupportedAudio(const QString &fileName);
  static bool isSupportedVideo(const QString &fileName);
  static bool isSupportedPicture(const QString &fileName);
  static bool isSupportedSubtitle(const QString &fileName);
  static bool isSupportedPlaylist(const QString &fileName);
  static bool isSupportedImage(const QString &fileName);
  static bool isSupportedFile(const QString &fileName);

  static QString defaultUserAgent();
  QString userAgent() const;

  // - Types -
public:

  enum Status { Stopped = 0, Playing = 1, Paused = 2 };

  struct MediaInfo {
    QString path; // MRL
    QString title;
    int track;

    MediaInfo() : track(0) { }
  };

  // - Constructions -
public:
  explicit Player(QObject *parent = nullptr);
  ~Player();

  bool isValid() const;

  //QString encoding() const;

  // - Queries -
public:
  bool isEmbedded() const;
  bool isFullScreen() const;
  bool hasMedia() const;
  bool hasRemoteMedia() const;
  bool isMouseEnabled() const;
  bool isKeyboardEnabled() const;
  bool isBufferSaved() const;
  bool isDownloadFinished() const;
  QString downloadPath() const;

  Status status() const;
  bool isPlaying() const;
  bool isPaused() const;
  bool isStopped() const;
  bool pausable() const;      // some media cannot be paused
  bool seekable() const;      // some media cannot be paused

  MediaInfo mediaInfo() const;
  QString mediaTitle() const;
  QString mediaPath() const;
  int trackNumber() const;

  int trackCount() const;
  bool hasPlaylist() const;
  QList<MediaInfo> playlist() const;

  QSize videoDimension() const;
  int videoCodecId() const;
  int audioCodecId() const;
  int audioChannelCount() const;
  int audioRate() const;

  QString videoCodecName() const { return codecName(videoCodecId()); }
  QString audioCodecName() const { return codecName(audioCodecId()); }

  static QString codecName(int codecId);

  /**
   *  \brief  Total play time in msecs.
   *
   *  When no media OR THE MEDIA WAS JUST OPENED, return -1.
   *  Due to deficiency of VLC, there is a delay between mediaChanged and lengthChanged signals.
   */
  qint64 mediaLength() const;
  qint64 mediaSize() const;

  ///  Current played time in msecs, always in [0, mediaLength()].
  qint64 time() const;

  qint64 audioDelay() const;

  // - Adjustment -
public:
  //bool isAdjustEnabled() const;
  qreal contrast() const;
  qreal brightness() const;
  int hue() const; // [0, 360]
  qreal saturation() const;
  qreal gamma() const;
public slots:
  //void setAdjustEnabled(bool t);
  void setContrast(qreal value);
  void resetContrast() { setContrast(1); }
  void setBrightness(qreal value);
  void resetBrightness() { setBrightness(1); }
  void setHue(int value);
  void resetHue() { setHue(0); }
  void setSaturation(qreal value);
  void resetSaturation() { setSaturation(1); }
  void setGamma(qreal value);
  void resetGamma() { setGamma(1); }

signals:
  //void adjustEnabledChanged(bool value);
  void contrastChanged(qreal value);
  void brightnessChanged(qreal value);
  void hueChanged(int value);
  void saturationChanged(qreal value);
  void gammaChanged(qreal value);

public:
#ifdef Q_OS_MAC
  typedef void *WindowHandle; ///< NSView
#else
  typedef WId WindowHandle;
#endif // Q_OS_MAC
  WindowHandle embeddedWindow() const;

public slots:
  void dispose();

  // Do not use WindowHandle to bypass qMetatype registration.
#ifdef Q_OS_MAC
  void setEmbeddedWindow(void *cocoaView);
#else
  void setEmbeddedWindow(WId winId);
#endif // Q_OS_MAC

  ///  A shortcut of setWinId for QWidget.
#ifdef Q_OS_MAC
  void embed(QMacCocoaViewContainer *w);
#else
  void embed(QWidget *w);
#endif // Q_OS_MAC

  void setDownloadsLocation(const QString &dir);

  void setBufferSaved(bool t);
  void saveBuffer();

  void setUserAgent(const QString &agent = QString());

  void setCookieJar(QNetworkCookieJar *jar);
  //void clearCookieJar() { setCookieJar(0); }

public:
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
  static QString searchSubtitleMedia(const QString &subtitleFile);
  static QStringList searchMediaSubtitles(const QString &mediaFile);

  bool hasTitles() const;
  int titleId() const;
  int titleCount() const;
  bool hasChapters() const;
  int chapterId() const;
  int chapterCount() const;
  int chapterCountForTitleId(int tid) const;

  int audioTrackCount() const;  ///< Number of audio tracks
  int audioTrackId() const;     ///< Return current audio track
  bool hasAudioTracks() const;
  QStringList audioTrackDescriptions() const; ///< Return list of available audio tracks

  // - Slots -
public slots:

  ///  All settings except parent and connections would be lost. Should be used for error recovery only.
  void reset();

  ///  Restore status before opening media. Should be used for error recovery only.
  void clearMedia();

  void setMediaTitle(const QString &t); ///< override default title

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
  bool openMedia(const QString &mrl);
  bool openMediaAsCD(const QString &url);
  void closeMedia();

  //bool openStream(const QStringList &mrls);
  void setStream(const QStringList &mrls, const QString &url, qint64 duration = 0);

  //void setEncoding(const QString &encoding); ///< See \c Encoding for details.

  bool openSubtitle(const QString &fileName); ///< Same as \c setSubtitleFromFile

  ///  Only work if embed/setWinId is not invoked.
  void setFullScreen(bool t = true);
  void toggleFullScreen();

  void setMouseEnabled(bool enabled = true);
  void setKeyboardEnabled(bool enabled = true);

  void setMouseEventEnabled(bool enabled = true);
  bool isMouseEventEnabled() const;

  void play();
  void stop();
  void pause();
  void resume();
  void playPause(); ///< Toggle play/pause.
  void nextFrame(); ///< Pause and jump to the next frame.

public:
  qreal rate() const;
  qreal volume() const;       ///< [0,1]
  qreal position() const;     ///< [0,1]
  qreal availablePosition() const; ///< [0,1]
  QString aspectRatio() const;
  qreal fps() const;
  qreal bitrate() const;

  ///  Consistent with ibvlc_audio_output_channel_t
  enum AudioChannel { NoChannel = 0, StereoChannel, ReverseStereoChannel, LeftChannel, RightChannel, DolbysChannel };
  AudioChannel audioChannel() const;

  // - Meta -
  QString metaTitle() const;
  QString metaArtist() const;
  QString metaGenre() const;
  QString metaCopyright() const;
  QString metaAlbum() const;
  QString metaTrackNumber() const;
  QString metaDescription() const;
  QString metaRating() const;
  QString metaDate() const;
  QString metaSetting() const;
  QString metaURL() const;
  QString metaLanguage() const;
  QString metaNowPlaying() const;
  QString metaPublisher() const;
  QString metaEncodedBy() const;
  QString metaArtworkURL() const;
  QString metaTrackID() const;

public slots:
  void setVolume(qreal vol);
  void setRate(qreal rate);
  void clearRate() { setRate(1.0); }
  void mute();
  void setPosition(qreal pos, bool checkPos = true);
  void setAspectRatio(const char *ratio); ///< ratio = width:height
  void setAspectRatio(const QString &ratio); ///< ratio = width:height
  void setAspectRatio(int width, int height);
  void clearAspectRatio();

  void setTime(qint64 msecs); ///< same effect as setPosition
  void setAudioDelay(qint64 msecs);
  void resetAudioDelay() { setAudioDelay(0); }

  void setSubtitleId(int id);   ///< id of available subtitles
  void showSubtitle();
  void hideSubtitle();
  void setSubtitleVisible(bool visible);

  void setAudioTrackId(int id);   ///< id of available subtitles

  void setAudioChannel(int channel);

  void setChapterId(int cid);
  void setPreviousChapter();
  void setNextChapter();
  void setTitleId(int tid);
  void setPreviousTitle();
  void setNextTitle();

  void updateTitleId();

  void loadExternalSubtitles(); ///< try to automatically load external subtitles

  void setTrackNumber(int track); ///< play track
  void setTrackNumber(); ///< default track is the current track number
  void nextTrack();
  void previousTrack();

public slots:
  ///  Only work if mouseEvent is enabled.
  void startVoutTimer();
  void stopVoutTimer();
protected slots:
  void updateVout(); ///< only works if vlccore is used
  void updateTrackInfo();
  void destroy();

public:
  void handleError(); ///< \internal

  // - Signals -
signals:
  void message(const QString &msg);
  void errorMessage(const QString &msg);
  void warning(const QString &msg);
  void fileSaved(const QString &fileName);
  void aspectRatioChanged(const QString &ratio);
  void downloadProgress(qint64 receivedBytes, qint64 totalBytes);
  void opening();
  void seeked();
  void buffering();
  void playing();
  void paused();
  void stopping();
  void stopped();
  void disposed();
  void mediaChanged();  // Actually mediaAdded
  void mediaTitleChanged(const QString &title);
  void mediaClosed();
  void timeChanged();
  void audioDelayChanged(qint64 msecs);
  void lengthChanged(); // Due to deficency of VLC, the length is changed after mediaChangd.
  void positionChanged();
  void volumeChanged();
  //void encodingChanged();
  void errorEncountered();
  void subtitleChanged();
  void audioTrackChanged();
  void audioChannelChanged(int channel);
  void endReached();

  // FIXME: This is not a good style to expose emit outside.
  // It could be better to use event to trigger emittion!!!!!!!!!!!!!!!!1
  // TODO: add a controlevent, and use it to communicate
  // Override event() to achieve this..
  //@{
  // \internal
public:
#define ADD_SIGNAL(_signal)    void emit_##_signal { emit _signal; } ///< \internal
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
    //ADD_SIGNAL(encodingChanged())
    ADD_SIGNAL(endReached())
    //ADD_SIGNAL(errorEncountered())
#undef ADD_SIGNAL
  //@}

signals:
  void rateChanged(qreal rate);
  void titleIdChanged(int tid);
  void trackNumberChanged(int track);

  // - Implementations -
public:
  Private *d() const { return d_; } ///< \internal

protected:
  //QByteArray decode(const QString &utf) const;
  //QString encode(const char *ascii) const;

  void attachEvents();
  void detachEvents();

private:
  QList<libvlc_media_t *> parsePlaylist(const QString &fileName) const;
  QList<libvlc_media_t *> parsePlaylist(libvlc_media_t *md) const;
};


/*
class PlayerListener : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(PlayerListener)
  typedef PlayerListener Self;
  typedef QObject Base;

  Player *player_;

public:
  explicit PlayerListener(Player *player, QObject *parent = nullptr);

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
