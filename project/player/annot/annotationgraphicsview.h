#ifndef ANNOTATIONGRAPHICSVIEW_H
#define ANNOTATIONGRAPHICSVIEW_H

// annotationview.h
// 7/14/2011

#include "module/annotcloud/annotation.h"
#include "module/qtext/eventlistener.h"
#include <QtGui/QGraphicsView>
#include <QtCore/QHash>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QTimer)

class SignalHub;
class Player;
class VideoView;
class AnnotationGraphicsItem;
class AnnotationEditor;
class AnnotationFilter;

///  An interactive shadow view.
class AnnotationGraphicsView : public QGraphicsView, public QtExt::EventListener
{
  Q_OBJECT
  typedef AnnotationGraphicsView Self;
  typedef QGraphicsView Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Types -
public:
  enum AnnotationPosition {
    AP_Top = 0, AP_Bottom
  };

  enum RenderHint {
    DefaultRenderHint = 0,
    TransparentHint,    // implies QGraphicsOpacityEffect
    ShadowHint,         // implies QGraphicsShadowEffect
    BlurHint,           // implies QGraphicsBlurEffect
    RenderHintCount
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
  void selectedUserIds(const QList<qint64> &ids);
  void hoveredItemPausedChanged(bool t);
  void hoveredItemResumedChanged(bool t);
  void hoveredItemRemovedChanged(bool t);
  void hoveredItemExiledChanged(bool t);
  void scaleChanged(qreal value);
  void rotationChanged(qreal value);
  void offsetChanged(qint64 value);
  void fullScreenModeChanged(bool);
public:
  /**
   *  Only used to measure the current full screen geometry.
   *  \param  w  any widget which is never null and is always in full screen mode
   */
  void setFullScreenView(QWidget *w) { fullScreenView_ = w; }
  bool isFullScreenMode() const { return fullScreen_; }
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
  void updateTrackingTimer();

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override stop polling when hidden

public:

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

  QRect globalRect() const;
  QPoint fromGlobal(const QPoint &globalPos) const;

signals:
  void annotationAdded(const Annotation &annot);
  //void annotationsChanged(const AnnotationList &annots);
  void annotationsRemoved();

public:
  bool isPaused() const { return paused_; }
  qint64 offset() const { return offset_; } ///< in seconds
signals:
  void paused();
  void resumed();
  void playbackEnabledChanged(bool enabled);
  void visibleChanged(bool visible);

  // - Properties -
public:
  AnnotationEditor *editor() const;
  bool hasPausedItems() const;

  RenderHint renderHint() const { return renderHint_; } ///< Default render hint

  void setFilter(AnnotationFilter *filter);

  int itemsCount() const;
  int itemsCount(int sec) const;
  int itemsCount(int from, int to) const; ///< Count of items within [from, to] in seconds

  bool isActive() const { return active_; }
  bool isPlaybackEnabled() const {  return playbackEnabled_; }

  qreal scale() const { return scale_; }
  qreal rotation() const { return rotation_; }

  bool isStarted() const;

  bool isHoveredItemPaused() const { return hoveredItemPaused_; }
  bool isHoveredItemResumed() const { return hoveredItemResumed_; }
  bool isHoveredItemRemoved() const { return hoveredItemRemoved_; }
  bool isHoveredItemExiled() const { return hoveredItemExiled_; }
public slots:
  void setScale(qreal value);
  void resetScale() { setScale(1.0); }

  void setOffset(qint64 secs) { emit offsetChanged(offset_ = secs); }
  void resetOffset() { setOffset(0); }

  void setRotation(qreal value);
  void resetRotation() { setRotation(0); }

  void setInterval(int msecs);
  void start();
  void stop();
  void tick();

  void setActive(bool active); // start polling when active
  void setPlaybackEnabled(bool enabled);
  void togglePlaybackEnabled() { setPlaybackEnabled(!playbackEnabled_); }
  void toggleVisible() { setVisible(!isVisible()); }

  void setHoveredItemPaused(bool t) { emit hoveredItemPausedChanged(hoveredItemPaused_ = t);}
  void setHoveredItemResumed(bool t) { emit hoveredItemResumedChanged(hoveredItemResumed_ = t);}
  void setHoveredItemRemoved(bool t) { emit hoveredItemRemovedChanged(hoveredItemRemoved_ = t);}
  void setHoveredItemExiled(bool t) { emit hoveredItemExiledChanged(hoveredItemExiled_ = t);}

  void setRenderHint(int hint);

  // Video view tracker
  void updateGeometry();
  void updateSize();
  void updatePos();

  // Stream control
  void updateCurrentTime();
  void invalidateAnnotations();       // reset annotations from database!

  void pause();
  void resume();

  void pauseItemAt(const QPoint &pos);
  void resumeItemAt(const QPoint &pos);
  void removeItemAt(const QPoint &pos);

  void pauseItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
  void resumeItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
  void removeItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);

  void pauseItems(const QPoint &pos);
  void resumeItems(const QPoint &pos);
  void removeItems(const QPoint &pos);

  void scalePausedItems(qreal scale);
  void rotatePausedItems(qreal angle);

  void exileItems(const QPoint &center);
  void exileItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);

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
  RenderHint renderHint_;
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
  qint64 currentTime_, // in secs
         offset_; // in secs
  int interval_; // in msecs, but will be rounded to seconds

  qint64 userId_;

  bool playbackEnabled_;
  AnnotationPosition subtitlePosition_;

  QList<qint64> filteredAnnotationIds_;

  qreal scale_, rotation_;

  bool hoveredItemPaused_, hoveredItemResumed_, hoveredItemRemoved_, hoveredItemExiled_;
};

#endif // ANNOTATIONGRAPHICSVIEW_H

