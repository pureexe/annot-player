#ifndef ANNOTATIONGRAPHICSViEW_H
#define ANNOTATIONGRAPHICSViEW_H

// annotationview.h
// 7/14/2011

#include "module/annotcloud/annotation.h"
#include "module/qtext/eventforwarder.h"
#include <QtGui/QGraphicsView>
#include <QtCore/QHash>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QTimer)

class AnnotationGraphicsItem;
class AnnotationGraphicsItemPool;
class AnnotationGraphicsItemScheduler;
class AnnotationEditor;
class AnnotationFilter;
class DataManager;
class Player;
class SignalHub;
class VideoView;

///  An interactive shadow view.
class AnnotationGraphicsView : public QGraphicsView, public QtExt::EventForwarder
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsView)
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
  AnnotationGraphicsView(SignalHub *hub, DataManager *data, Player *player, VideoView *videoView, QWidget *parent = 0);
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

  bool isItemCountLimited() const { return itemCountLimited_; }

signals:
  void message(const QString &text);
  void searchRequested(int engine, const QString &key);
  void translateRequested(const QString &text, int lang);
  void traditionalChineseRequested(const QString &text);
  void annotationSkipped();
  void itemCountLimitedChanged(bool t);
  void itemMetaVisibleChanged(bool t);
  void selectedUserId(qint64 uid);
  void selectedUserIds(const QList<qint64> &ids);
  void hoveredItemPausedChanged(bool t);
  void hoveredItemResumedChanged(bool t);
  void hoveredItemRemovedChanged(bool t);
  void nearbyItemExpelledChanged(bool t);
  void nearbyItemAttractedChanged(bool t);
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
  WId trackedWindow() const { return trackedWindow_; }
public slots:
  void setTrackedWindow(WId winId);

signals:
  void removeItemRequested();
  void annotationPosChanged(qint64 msecs); ///< current pos changed by show at pos, not the pos of the widget

  void subtitleAdded(const QString &richText);
  void annotationAdded(const QString &richText);

  void posChanged(); ///< manually moved
  void sizeChanged(); ///< manually resized
  void annotationTextSubmitted(const QString &text);
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
  virtual void setVisible(bool visible); ///< \reimp stop polling when hidden

public:
  // Implement EventForwarder.
  virtual void sendContextMenuEvent(QContextMenuEvent *event); ///< \reimp
  virtual void sendMouseMoveEvent(QMouseEvent *event); ///< \reimp
  virtual void sendMousePressEvent(QMouseEvent *event); ///< \reimp
  virtual void sendMouseReleaseEvent(QMouseEvent *event); ///< \reimp
  virtual void sendMouseDoubleClickEvent(QMouseEvent *event); ///< \reimp

protected:
  //virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp
  //virtual void mouseMoveEvent(QMouseEvent *event); ///< \reimp
  //virtual void mousePressEvent(QMouseEvent *event); ///< \reimp
  //virtual void mouseReleaseEvent(QMouseEvent *event); ///< \reimp
  //virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \reimp

protected:
  void moveToGlobalPos(const QPoint &globalPos);

  AnnotationGraphicsItem *itemWithId(qint64 aid) const;
  Annotation *annotationWithId(qint64 aid) const;

public:
  ///  Generate list of annotations in the view (slow).
  AnnotationList annotations() const;
  const AnnotationList &annotationsAtPos(qint64 pos) const; ///< slow

  QRect globalRect() const;
  QPoint fromGlobal(const QPoint &globalPos) const;

  void searchText(const QString &text, int engine);
  void translateText(const QString &text, int lang);
  void showTraditionalChinese(const QString &text);

signals:
  void annotationAdded(const Annotation &annot);
  //void annotationsChanged(const AnnotationList &annots);
  void annotationsRemoved();

public:
  bool isPaused() const { return paused_; }
  int offset() const { return offset_; } ///< in seconds
signals:
  void paused();
  void resumed();
  void playbackEnabledChanged(bool enabled);
  void visibleChanged(bool visible);

  void itemVisibleChanged(bool visble);

  // - Properties -
public:
  AnnotationEditor *editor() const;
  AnnotationGraphicsItemScheduler *scheduler() const { return scheduler_; }
  bool hasPausedItems() const;

  RenderHint renderHint() const { return renderHint_; } ///< Default render hint

  void setFilter(AnnotationFilter *filter) { filter_ = filter; }

  int itemsCount() const;
  int itemsCount(int sec) const;
  int itemsCount(int from, int to) const; ///< Count of items within [from, to] in seconds

  bool isPlaybackEnabled() const {  return playbackEnabled_; }

  qreal scale() const { return scale_; }
  qreal rotation() const { return rotation_; }

  bool isStarted() const;

  void selectItem(AnnotationGraphicsItem *item, bool detail = false);

  bool isHoveredItemPaused() const { return hoveredItemPaused_; }
  bool isHoveredItemResumed() const { return hoveredItemResumed_; }
  bool isHoveredItemRemoved() const { return hoveredItemRemoved_; }
  bool isNearbyItemExpelled() const { return nearbyItemExpelled_; }
  bool isNearbyItemAttracted() const { return nearbyItemAttracted_; }

  bool isItemMetaVisible() { return metaVisible_; }

public slots:
  void setItemMetaVisible(bool visible)
  { if (metaVisible_ != visible) emit itemMetaVisibleChanged(metaVisible_ = visible); }

  void setItemVisible(bool visible)
  { emit itemVisibleChanged( itemVisible_ = visible); }

  void setItemCountLimited(bool t)
  { emit itemCountLimitedChanged(itemCountLimited_ = t); }

  void setScale(qreal value);

  void setOffset(int secs) { emit offsetChanged(offset_ = secs); }

  void setRotation(qreal value);

  void setInterval(int msecs) {  interval_ = msecs; Q_ASSERT(interval_ >= 1000); } // at least 1 secons
  void start();
  void stop();
  void tick();

  void setPlaybackEnabled(bool enabled);
  void togglePlaybackEnabled() { setPlaybackEnabled(!playbackEnabled_); }
  void toggleVisible() { setVisible(!isVisible()); }

  void setHoveredItemPaused(bool t) { emit hoveredItemPausedChanged(hoveredItemPaused_ = t);}
  void setHoveredItemResumed(bool t) { emit hoveredItemResumedChanged(hoveredItemResumed_ = t);}
  void setHoveredItemRemoved(bool t) { emit hoveredItemRemovedChanged(hoveredItemRemoved_ = t);}
  void setNearbyItemExpelled(bool t) { emit nearbyItemExpelledChanged(nearbyItemExpelled_ = t);}
  void setNearbyItemAttracted(bool t) { emit nearbyItemAttractedChanged(nearbyItemAttracted_ = t);}

  void setRenderHint(int hint);

  // Video view tracker
  void updateGeometry();
  void updateSize();
  void updateMaxItemCount();
  void updatePos();

  // Stream control
  void updateCurrentTime();
  void invalidateAnnotations();

  void pause();
  void resume();

  void releaseItem(AnnotationGraphicsItem *item);

  void pauseItemAt(const QPoint &pos);
  void resumeItemAt(const QPoint &pos);
  void removeItemAt(const QPoint &pos);

  void pauseItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
  void resumeItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
  void removeItems(const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);

  void pauseItems(const QPoint &pos);
  void resumeItems(const QPoint &pos);
  void removeItems(const QPoint &pos);
  void removeAllItems();

  void scalePausedItems(qreal scale);
  void rotatePausedItems(qreal angle);

  void expelItems(const QPoint &center);
  void expelItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
  void expelAllItems(const QPoint &center);

  void attractItems(const QPoint &center);
  void attractAllItems(const QPoint &center);
  void attractItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);

  // Annotations

  /**
   *  When delay is -1, \param anno will not be shown.
   *  Otherwise, it will be shown if it is within [0, delay] seconds.
   *  Specally, LLONG_MAX will cause it always get displayed if annot is in the past.
   */
  void addAnnotation(const Annotation &annot, qint64 delaysecs = 1);
  void addAndShowAnnotation(const Annotation &annot); ///< a shortcut for above with LLONG_MAX
  ///  Display without add annotation.
  void showAnnotation(const Annotation &annot, bool showMeta = true);
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

  bool isItemVisible() const { return itemVisible_; }

  // - Implementations -
private:
  AnnotationGraphicsItemPool *pool_;
  VideoView *videoView_;
  QWidget *fullScreenView_;
  WId trackedWindow_;
  AnnotationEditor *editor_;
  AnnotationGraphicsItemScheduler *scheduler_;
  SignalHub *hub_;
  DataManager *data_;
  Player *player_;
  AnnotationFilter *filter_;
  RenderHint renderHint_;
  bool paused_;
  bool fullScreen_;
  bool subtitleVisible_, nonSubtitleVisible_;
  bool metaVisible_;
  bool itemCountLimited_;

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

  bool hoveredItemPaused_, hoveredItemResumed_, hoveredItemRemoved_,
       nearbyItemExpelled_, nearbyItemAttracted_;
  bool itemVisible_;
  bool dragging_;
  int maxItemCount_;
};

#endif // ANNOTATIONGRAPHICSViEW_H

