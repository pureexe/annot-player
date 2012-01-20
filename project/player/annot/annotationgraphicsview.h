#ifndef ANNOTATIONGRAPHICSVIEW_H
#define ANNOTATIONGRAPHICSVIEW_H

// annotationview.h
// 7/14/2011

#include "core/eventlistener.h"
#include "core/cloud/annotation.h"
#include <QGraphicsView>
#include <QHash>
#include <QList>

QT_FORWARD_DECLARE_CLASS(QTimer)

class SignalHub;
class Player;
class VideoView;
class AnnotationGraphicsItem;
class AnnotationEditor;
class AnnotationFilter;

///  An interactive shadow view.
class AnnotationGraphicsView : public QGraphicsView, public Core::EventListener
{
  Q_OBJECT
  typedef AnnotationGraphicsView Self;
  typedef QGraphicsView Base;

  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  // - Types -
public:
  enum AnnotationPosition {
    AP_Top = 0, AP_Bottom
  };

  // - Constructions -
public:
  AnnotationGraphicsView(SignalHub *hub, Player *player, VideoView *videoView, QWidget *parent = 0);
  //~AnnotationGraphicsView();

  // - Properties -
public:
  qint64 userId() const { return userId_; }
  void setUserId(qint64 uid) { userId_ = uid; }

  AnnotationPosition subtitlePosition() const { return subtitlePosition_; }
  void setSubtitlePosition(AnnotationPosition ap) { subtitlePosition_ = ap; }

  void setSubtitlePrefix(const QString &prefix) { subtitlePrefix_ = prefix; }
  const QString &subtitlePrefix() const { return subtitlePrefix_; }

  bool isSubtitleVisible() const { return subtitleVisible_; }
  bool isNonSubtitleVisible() const { return nonSubtitleVisible_; }

signals:
  void fullScreenModeChanged(bool);
public:
  /**
   *  Only used to measure the current full screen geometry.
   *  \param  w  any widget which is never null and is always in full screen mode
   */
  void setFullScreenView(QWidget *w);
  bool isFullScreenMode() const;
public slots:
  void setFullScreenMode(bool t = true);

  void setSubtitleVisible(bool t) { subtitleVisible_ = t; }
  void setNonSubtitleVisible(bool t) { nonSubtitleVisible_ = t; }

  /**
   *  Set the windows to track.
   *  Currently only works on Windows.
   */
signals:
  void trackedWindowDestroyed();
  void trackedWindowChanged(WId winId);
public:
  WId trackedWindow() const;
public slots:
  void setTrackedWindow(WId winId);

signals:
  void removeItemRequested();
  void annotationPosChanged(qint64 msecs); ///< current pos changed by show at pos, not the pos of the widget

  void subtitleAdded(const QString &richText);
  void annotationAdded(const QString &richText);

  void posChanged(); ///< manually moved
  void sizeChanged(); ///< manually resized
  void annotationTextUpdatedWithId(const QString &text, qint64 id);
  void annotationDeletedWithId(qint64 id);

  void userBlessedWithId(qint64 uid);
  void userCursedWithId(qint64 uid);
  void userBlockedWithId(qint64 uid);
  void userBlockedWithAlias(QString alias);

  void annotationBlessedWithId(qint64 aid);
  void annotationCursedWithId(qint64 aid);
  void annotationBlockedWithId(qint64 aid);
  void annotationBlockedWithText(QString alias);

public slots:
  void blessUserWithId(qint64 uid) { emit userBlessedWithId(uid); }
  void curseUserWithId(qint64 uid) { emit userCursedWithId(uid); }
  void blockUserWithId(qint64 uid) { emit userBlockedWithId(uid); }
  void blockUserWithAlias(QString alias) { emit userBlockedWithAlias(alias); }

  void blessAnnotationWithId(qint64 uid) { emit annotationBlessedWithId(uid); }
  void curseAnnotationWithId(qint64 uid) { emit annotationCursedWithId(uid); }
  void blockAnnotationWithId(qint64 uid) { emit annotationBlockedWithId(uid); }
  void blockAnnotationWithText(QString text) { emit annotationBlockedWithText(text); }

protected slots:
  void startTracking();
  void stopTracking();
  void invalidateTrackingTimer();

public:
  // - Events -
  virtual void setVisible(bool visible); ///< \override stop polling when hidden

public:
  QRect globalRect() const;

  // Implement event listener.
  virtual void sendContextMenuEvent(QContextMenuEvent *event); ///< \override
  virtual void sendMouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void sendMousePressEvent(QMouseEvent *event); ///< \override
  virtual void sendMouseReleaseEvent(QMouseEvent *event); ///< \override
  virtual void sendMouseDoubleClickEvent(QMouseEvent *event); ///< \override

protected:
  void connectPlayer();
  void disconnectPlayer();
  void connectHub();
  void disconnectHub();

  void moveToGlobalPos(const QPoint &globalPos);

  AnnotationGraphicsItem *itemWithId(qint64 aid) const;
  Annotation *annotationWithId(qint64 aid) const;

public:
  ///  Generate list of annotations in the view (slow).
  AnnotationList annotations() const;

  const AnnotationList &annotationsAtPos(qint64 pos) const; ///< slow

signals:
  void annotationAdded(const Annotation &annot);
  //void annotationsChanged(const AnnotationList &annots);
  void annotationsRemoved();

public:
  bool isPaused() const;
signals:
  void paused();
  void resumed();
  void playbackEnabledChanged(bool enabled);
  void visibleChanged(bool visible);

  // - Properties -
public:
  AnnotationEditor *editor() const;

  void setFilter(AnnotationFilter *filter);

  int itemsCount() const;
  int itemsCount(int sec) const;
  int itemsCount(int from, int to) const; ///< Count of items within [from, to] in seconds

  bool isActive() const { return active_; }
  bool isPlaybackEnabled() const {  return playbackEnabled_; }

  bool isStarted() const;
public slots:
  void setInterval(int msecs);
  void start();
  void stop();
  void tick();

  void setActive(bool active); // start polling when active
  void setPlaybackEnabled(bool enabled);
  void togglePlaybackEnabled() { setPlaybackEnabled(!playbackEnabled_); }
  void toggleVisible() { setVisible(!isVisible()); }


  // Video view tracker
  void invalidateGeometry();
  void invalidateSize();
  void invalidatePos();

  // Stream control
  void invalidateCurrentTime();
  void invalidateAnnotations();       // reset annotations from database!

  void pause();
  void resume();

  // Annotations

  /**
   *  When delay is -1, \param anno will not be shown.
   *  Otherwise, it will be shown if it is within [0, delay] seconds.
   *  Specally, LLONG_MAX will cause it always get displayed if annot is in the past.
   */
  void addAnnotation(const Annotation &annot, qint64 delaysecs = 1);
  void addAndShowAnnotation(const Annotation &annot); ///< a shortcut for above with LLONG_MAX
  ///  Display without add annotation.
  void showAnnotation(const Annotation &annot);
  void addAnnotations(const AnnotationList &annots);
  void setAnnotations(const AnnotationList &annots);
  void showAnnotationOnce(const Annotation &annot);

  ///  Add at [currentTime, currentTime+interval/1000)
  void appendAnnotations(const AnnotationList &annots);

  // Message mode:
  void showAnnotationsAtPos(qint64 pos);

  // TODO
  //void addAnnotations(const Core::AnnotationList &l);

  ///  Emit deletion signal if deleteAnnot is true
  void removeAnnotationWithId(qint64 id, bool deleteAnnot = false);
  void removeAnnotations(); // delete annotation without reading ones

protected slots:
  void updateAnnotationText(const QString &text);
  void updateAnnotationTextWithId(const QString &text, qint64 aid);

public:
  bool isAnnotationBlocked(const Annotation &a) const;
  bool isItemBlocked(const AnnotationGraphicsItem *item) const;

  // - Implementations -
private:
  VideoView *videoView_;
  QWidget *fullScreenView_;
  WId trackedWindow_;
  AnnotationEditor *editor_;
  SignalHub *hub_;
  Player *player_;
  AnnotationFilter *filter_;
  bool active_;
  bool paused_;
  bool fullScreen_;
  bool subtitleVisible_, nonSubtitleVisible_;

  QString subtitlePrefix_;

  QTimer *timer_; // live timer
  QTimer *trackingTimer_;

  // Though not quint64, the time is not supposed to be negative.
  //QHash<qint64, QList<AnnotationGraphicsItem*>*> annots_; // indexed by secs for MediaMode or hash for SignalMode
  typedef QHash<qint64, AnnotationList> AnnotationHash;
  AnnotationHash hash_; // indexed by secs for MediaMode or hash for SignalMode
  qint64 currentTime_; // in secs
  int interval_; // in msecs, but will be rounded to seconds

  qint64 userId_;

  bool playbackEnabled_;
  AnnotationPosition subtitlePosition_;

  QList<qint64> filteredAnnotationIds_;
};

#endif // ANNOTATIONGRAPHICSVIEW_H

