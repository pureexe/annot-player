#ifndef SETTINGS_H
#define SETTINGS_H

// settings.h
// 7/30/2011

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtGui/QColor>

class Settings : public QSettings
{
  Q_OBJECT
  Q_DISABLE_COPY(Settings)
  typedef Settings Self;
  typedef QSettings Base;

  // - Constructions -
public:
  static Self *globalSettings() { static Self g; return &g; }
protected:
  explicit Settings(QObject *parent = nullptr);

public slots:
  void sync() { Base::sync(); }

  // - Properties -
public:
  QString version() const;
  void setVersion(const QString &version);

  QString applicationFilePath() const;
  void setApplicationFilePath(const QString &path);

  bool isQueueEmpty() const; ///< If database queue is empty
  void setQueueEmpty(bool empty);

  bool isBufferedMediaSaved() const;
  void setBufferedMediaSaved(bool t);

  bool isAnnotationFileSaved() const;
  void setAnnotationFileSaved(bool t);

  bool isAutoSubmit() const;
  void setAutoSubmit(bool t);

  bool isTranslateEnabled() const; ///< If use translation in game mode
  void setTranslateEnabled(bool enabled);

  bool isSubtitleOnTop() const;
  void setSubtitleOnTop(bool t);

  bool isWindowOnTop() const;
  void setWindowOnTop(bool t);

  bool isEmbeddedPlayerOnTop() const;
  void setEmbeddedPlayerOnTop(bool t);

  bool isPlayerLabelEnabled() const;
  void setPlayerLabelEnabled(bool t);

  bool isMenuBarVisible() const;
  void setMenuBarVisible(bool t);

  bool isAutoPlayNext() const;
  void setAutoPlayNext(bool t);

  bool translateSelection() const;
  void setTranslateSelection(bool t);

  bool showLibrary() const;
  void setShowLibrary(bool t);

  int libraryView() const;
  void setLibraryView(int id);

  bool isLive() const;
  void setLive(bool t);

  bool preferLocalDatabase() const;
  void setPreferLocalDatabase(bool t);

  QDate updateDate() const;
  void setUpdateDate(const QDate &date);

  int subtitleColor() const;
  void setSubtitleColor(int colorId);

  QColor annotationHighlightColor() const;
  void setAnnotationHighlightColor(const QColor &color);

  QColor annotationOutlineColor() const;
  void setAnnotationOutlineColor(const QColor &value);

  QColor subtitleOutlineColor() const;
  void setSubtitleOutlineColor(const QColor &value);

  int annotationEffect() const;
  void setAnnotationEffect(int effect);

  int annotationPositionResolution() const;
  void setAnnotationPositionResolution(int value);

  qreal annotationSpeedup() const;
  void setAnnotationSpeedup(qreal value);

  QString annotationFontFamily() const;
  void setAnnotationFontFamily(const QString &family);

  QString annotationJapaneseFontFamily() const;
  void setAnnotationJapaneseFontFamily(const QString &family);

  QString annotationChineseFontFamily() const;
  void setAnnotationChineseFontFamily(const QString &family);

  qint64 annotationOffset() const;
  void setAnnotationOffset(qint64 offset);

  bool preferFloatAnnotation() const;
  void setPreferFloatAnnotation(bool t);

  bool preferTraditionalChinese() const;
  void setPreferTraditionalChinese(bool t);

  ///  Theme in UiStyle
  QSet<int> annotationLanguages() const;
  void setAnnotationLanguages(const QSet<int> languages);

  void setMultipleWindowsEnabled(bool t);
  bool isMultipleWindowsEnabled() const;

  QStringList recentFiles() const;
  void setRecentFiles(const QStringList &files);
  //void clearRecentFiles();

  QHash<QString,QString> recentTitles() const;
  void setRecentTitles(const QHash<QString,QString> &titles);

  QHash<QString,QString> gameEncodings() const;
  void setGameEncodings(const QHash<QString,QString> &l, int limit = 0);

  QString recentPath() const;
  void setRecentPath(const QString &path);

  bool isAnnotationAvatarVisible() const;
  void setAnnotationAvatarVisible(bool t);

  bool isAnnotationMetaVisible() const;
  void setAnnotationMetaVisible(bool t);

  bool isAnnotationBandwidthLimited() const;
  void setAnnotationBandwidthLimited(bool t);

  bool isAnnotationFilterEnabled() const;
  void setAnnotationFilterEnabled(bool t);

  QStringList blockedKeywords() const;
  void setBlockedKeywords(const QStringList &l);

  QStringList blockedUserNames() const;
  void setBlockedUserNames(const QStringList &l);

  int annotationCountHint() const;
  void setAnnotationCountHint(int count);

  // - Service providers -

  ulong translationServices() const;
  void setTranslationServices(ulong services);

  // - History per token -

  QHash<qint64, qint64> playPosHistory() const;
  void setPlayPosHistory(const QHash<qint64, qint64> &h);

  QHash<qint64, int> subtitleHistory() const;
  void setSubtitleHistory(const QHash<qint64, int> &h);

  QHash<qint64, int> audioTrackHistory() const;
  void setAudioTrackHistory(const QHash<qint64, int> &h);

  QHash<qint64, int> audioChannelHistory() const;
  void setAudioChannelHistory(const QHash<qint64, int> &h);

  QHash<qint64, QString> aspectRatioHistory() const;
  void setAspectRatioHistory(const QHash<qint64, QString> &h);

  // - Video control -
  int hue() const; void setHue(int value);
  qreal contrast() const; void setContrast(qreal value);
  qreal gamma() const; void setGamma(qreal value);
  qreal saturation() const; void setSaturation(qreal value);
  qreal brightness() const; void setBrightness(qreal value);

  // - Annotation transforms -
  int annotationOpacityFactor() const; void setAnnotationOpacityFactor(int value);
  int annotationBackgroundOpacityFactor() const; void setAnnotationBackgroundOpacityFactor(int value);
  qreal annotationScale() const; void setAnnotationScale(qreal value);
  qreal annotationFullscreenScale() const; void setAnnotationFullscreenScale(qreal value);
  //qreal annotationRotation() const; void setAnnotationRotation(qreal value);

  // - Game -

  bool isAppLocaleEnabled() const;
  void setAppLocaleEnabled(bool t);

  bool isGameTextVisible() const;
  void setGameTextVisible(bool t);

  bool isGameTextColorful() const;
  void setGameTextColorful(bool t);

  bool isGameTextResizable() const;
  void setGameTextResizable(bool t);

  bool isAtlasEnabled() const;
  void setAtlasEnabled(bool value);
};

#endif // SETTINGS_H
