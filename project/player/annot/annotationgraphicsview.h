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
  explicit AnnotationGraphicsView(SignalHub *hub, Player *player, VideoView *videoView, QWidget *parent = 0);
  ~AnnotationGraphicsView();

  // - Properties -
public:
  qint64 userId() const;
  void setUserId(qint64 uid);

  AnnotationPosition subtitlePosition() const;
  void setSubtitlePosition(AnnotationPosition ap);

  void setSubtitlePrefix(const QString &prefix);
  const QString &subtitlePrefix() const;

  /**
   *  Only used to measure the current full screen geometry.
   *  \param  w  any widget which is never null and is always in full screen mode
   */
  void setFullScreenView(QWidget *w);

  /**
   *  Set the windows to track.
   *  Currently only works on Windows.
   */
signals:
  void trackingWindowChanged(WId winId);
public:
  WId trackingWindow() const;
public slots:
  void setTrackingWindow(WId winId);

signals:
  void posChanged(); ///< current pos changed by show at pos, not the pos of the widget
  void annotationTextUpdated(const QString &text, qint64 id);

public slots:
  void invalidateAnnotationPos(); ///< emit posChanged

protected slots:
  void startTracking();
  void stopTracking();

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

//public:
//  qint64 playTime() const;
//  void setPlayTime(qint64 sec);

  ///  Generate list of annotations in the view (slow).
  AnnotationList annotations() const;

  AnnotationList annotationsAtPos(qint64 pos) const; ///< slow

public:
  bool isPaused() const;
signals:
  void paused();
  void resumed();
  void playbackEnabledChanged(bool enabled);

  // - Properties -
public:
  AnnotationEditor *editor() const;

  void setFilter(AnnotationFilter *filter);

  int itemsCount() const;
  int itemsCount(int sec) const;
  int itemsCount(int from, int to) const; ///< Count of items within [from, to] in seconds

  bool isActive() const;
  bool isPlaybackEnabled() const;
public slots:
  void setActive(bool active); // start polling when active
  void setPlaybackEnabled(bool enabled);
  void togglePlaybackEnabled();

  // Video view tracker
  void invalidateGeometry();
  void invalidateSize();
  void invalidatePos();

  // Stream control
  void invalidatePlayTime();
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

  // Message mode:
  void showAnnotationsAtPos(qint64 pos);

  // TODO
  //void addAnnotations(const Core::AnnotationList &l);

protected slots:
  void clearAnnotations(); // delete annotation without reading ones

  void updateAnnotationText(const QString &text);

public:
  bool isItemFiltered(const AnnotationGraphicsItem *item) const;

  // - Implementations -
private:
  VideoView *videoView_;
  QWidget *fullScreenView_;
  WId trackingWindow_;
  SignalHub *hub_;
  Player *player_;
  AnnotationFilter *filter_;
  bool active_;
  bool paused_;

  QString subtilePrefix_;

  QTimer *trackingTimer_;

  AnnotationEditor *editor_;

  // Though not quint64, the time is not supposed to be negative.
  QHash<qint64, QList<AnnotationGraphicsItem*>*> annots_; // indexed by secs for MediaMode or hash for SignalMode
  qint64 playTime_; // in sec

  qint64 userId_;

  bool playbackEnabled_;
  AnnotationPosition subtitlePosition_;
};

#endif // ANNOTATIONGRAPHICSVIEW_H

