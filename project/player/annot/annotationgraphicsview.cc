// annotationgraphicsview.cc
// 7/14/2011

#include "annotationgraphicsview.h"
#include "annotationgraphicsitem.h"
#include "annotationgraphicsitempool.h"
#include "annotationgraphicsitemscheduler.h"
#include "annotationsettings.h"
#include "annotationeditor.h"
#include "annotationfilter.h"
#include "stylesheet.h"
#include "signalhub.h"
#include "videoview.h"
#include "global.h"
#include "application.h"
#include "project/common/acss.h"
#include "module/player/player.h"
#include "module/qtext/htmltag.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef WITH_WIN_MOUSEHOOK
# include "win/mousehook/mousehook.h"
#endif // WITH_WIN_MOUSEHOOK
#include <QtGui>
#ifdef __clang__
# pragma clang diagnostic ignored "-Wunused-parameter" // in boost algorithm
#endif // __clang__
#include <boost/range/algorithm.hpp>
#include <climits>

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

//#define DEBUG "annotationgraphicsview"
#include "module/debug/debug.h"

using namespace AnnotCloud;

#define MAX_SUBTITLE_HISTORY    30
#define TIMER_INTERVAL        1000 // 1 second

enum { ReservedItemCount = 10 };

// - Construction -

AnnotationGraphicsView::AnnotationGraphicsView(
  SignalHub *hub,
  DataManager *data,
  Player *player,
  VideoView *view,
  QWidget *parent)
  : Base(parent), videoView_(view), fullScreenView_(nullptr), trackedWindow_(0), editor_(nullptr),
    hub_(hub), data_(data), player_(player),
    filter_(nullptr), renderHint_(DefaultRenderHint), paused_(false), fullScreen_(false),
    subtitleVisible_(true), nonSubtitleVisible_(true), metaVisible_(false), itemCountLimited_(true),
    currentTime_(-1), offset_(0), interval_(TIMER_INTERVAL), userId_(0), playbackEnabled_(true),
    scale_(-1), // invalid
    rotation_(0),
    hoveredItemPaused_(false), hoveredItemResumed_(false), hoveredItemRemoved_(false), nearbyItemExpelled_(false), nearbyItemAttracted_(false),
    itemVisible_(true), dragging_(false), maxItemCount_(ReservedItemCount)
{
  Q_ASSERT(hub_);
  Q_ASSERT(player_);
  Q_ASSERT(videoView_);

  QGraphicsScene *scene = new QGraphicsScene(this);
  {
    scene->setItemIndexMethod(QGraphicsScene::NoIndex); // moving items are not indexed
  }
  setScene(scene);
  {
    //setMouseTracking(true);
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet(ACSS_GRAPHICSVIEW);
    setWindowOpacity(0); // disable background

    //setAttribute(Qt::WA_TransparentForMouseEvents);
    //setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeAnchor(NoAnchor);
    setTransformationAnchor(NoAnchor);
    setAlignment(Qt::AlignRight | Qt::AlignTop);
    //setCacheMode(QGraphicsView::CacheBackground);
    setDragMode(QGraphicsView::NoDrag);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setOptimizationFlags(
      QGraphicsView::DontSavePainterState |
      QGraphicsView::DontAdjustForAntialiasing
    );
    //setRenderHints( // Alias disabled to improve performance
    //  //QPainter::Antialiasing |
    //  //QPainter::TextAntialiasing |
    //  QPainter::SmoothPixmapTransform
    //);
  }

  scheduler_ = new AnnotationGraphicsItemScheduler(hub_, this);
  connect(this, SIGNAL(scaleChanged(qreal)), scheduler_, SLOT(setScale(qreal)));
  connect(this, SIGNAL(annotationsRemoved()), scheduler_, SLOT(clear()));
  connect(this, SIGNAL(sizeChanged(QSize)), scheduler_, SLOT(setViewSize(QSize)));

  pool_ = new AnnotationGraphicsItemPool(this, data_, hub_, this);
  pool_->reserve(ReservedItemCount);

  timer_ = new QTimer(this);
  timer_->setInterval(TIMER_INTERVAL);
  connect(timer_, SIGNAL(timeout()), SLOT(tick()));

  trackingTimer_ = new QTimer(this);
  trackingTimer_->setInterval(G_TRACKING_INTERVAL);
  connect(trackingTimer_, SIGNAL(timeout()), SLOT(updateGeometry()));

  connect(hub_, SIGNAL(windowModeChanged(SignalHub::WindowMode)), SLOT(updateScale()));

  connect(this, SIGNAL(offsetChanged(qint64)), SLOT(removeAllItems()));

  connect(AnnotationSettings::globalSettings(), SIGNAL(offsetChanged(int)), SLOT(setOffset(int)));
  connect(AnnotationSettings::globalSettings(), SIGNAL(scaleChanged(qreal)), SLOT(updateScale()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(fullscreenScaleChanged(qreal)), SLOT(updateScale()));
  connect(AnnotationSettings::globalSettings(), SIGNAL(rotationChanged(qreal)), SLOT(setRotation(qreal)));

  connect(player_, SIGNAL(mediaClosed()), SLOT(invalidateAnnotations()));
  connect(player_, SIGNAL(timeChanged()), SLOT(updateCurrentTime()));
  connect(player_, SIGNAL(paused()), SLOT(pause()));
  connect(player_, SIGNAL(playing()), SLOT(resume()));
  connect(player_, SIGNAL(stopped()), SLOT(resume()));
  connect(player_, SIGNAL(stopped()), SLOT(removeAllItems()));
  connect(player_, SIGNAL(seeked()), SLOT(removeAllItems()));
  connect(player_, SIGNAL(rateChanged(qreal)), SLOT(removeAllItems()));
  connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), SLOT(invalidateAnnotations()));
  connect(hub_, SIGNAL(paused()), SLOT(pause()));
  connect(hub_, SIGNAL(played()), SLOT(resume()));
  connect(hub_, SIGNAL(stopped()), SLOT(resume()));
  connect(hub_, SIGNAL(stopped()), SLOT(removeAllItems()));

  updateScale();

  //centerOn(0, 0);
}

// - Properties -

void
AnnotationGraphicsView::setRenderHint(int hint)
{
  switch (hint) {
  case TransparentHint: renderHint_ = TransparentHint; break;
  case ShadowHint: renderHint_ = ShadowHint; break;
  case BlurHint: renderHint_ = BlurHint; break;
  case DefaultRenderHint:
  default:
    renderHint_ = DefaultRenderHint;
  }
}

//AnnotationGraphicsView::~AnnotationGraphicsView()
//{ removeAnnotations(); }

AnnotationEditor*
AnnotationGraphicsView::editor() const
{
  if (!editor_) {
    Self *self = const_cast<Self *>(this);
    AnnotationEditor *ret = new AnnotationEditor(self);
    connect(ret, SIGNAL(textSaved(QString)), SLOT(updateAnnotationText(QString)));
    self->editor_ = ret;
  }

  return editor_;
}

void
AnnotationGraphicsView::setFullScreenMode(bool t)
{
  if (fullScreen_ != t) {
    fullScreen_ = t;
    updateGeometry();
    updateTrackingTimer();
    emit fullScreenModeChanged(fullScreen_);
  }
}

void
AnnotationGraphicsView::removeAnnotationWithId(qint64 id, bool deleteAnnot)
{
  if (id && !hash_.empty()) {
    AnnotationGraphicsItem *item = itemWithId(id);
    if (item)
      item->disappear();

    bool found = false;
    for (auto h = hash_.begin(); h != hash_.end() && !found; ++h) {
      AnnotationList &l = h.value();
      for (auto p = l.begin(); p != l.end() && !found; ++p)
        if (p->id() == id) {
          l.erase(p);
          found = true;
        }
    }

    if (found && deleteAnnot)
      emit annotationDeletedWithId(id);

    //bool found = false;
    //foreach (QList<AnnotationGraphicsItem*> *l, annots_.values()) {
    //  if (l && !l->empty())
    //    foreach (AnnotationGraphicsItem *item, *l)
    //      if (item && item->annotation().id() == id) {
    //        l->removeAll(item);
    //        QTimer::singleShot(0, item, SLOT(deleteLater()));
    //        found = true;
    //        break;
    //      }
    //
    //  if (found) {
    //    if (deleteAnnot)
    //      emit annotationDeletedWithId(id);
    //    break;
    //  }
    //}
  }
}

void
AnnotationGraphicsView::removeAnnotations()
{
  removeAllItems();
  hash_.clear();
  emit annotationsRemoved();
}

void
AnnotationGraphicsView::setAnnotations(const AnnotationList &annots)
{
  removeAnnotations();
  addAnnotations(annots);
}

AnnotationList
AnnotationGraphicsView::annotations() const
{
  AnnotationList ret;
  //if (!annots_.empty())
  //  foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
  //    if (l && !l->empty())
  //      foreach (AnnotationGraphicsItem *item, *l)
  //        if (item)
  //          ret.append(item->annotation());
  if (!hash_.empty())
    for (auto p = hash_.constBegin(); p != hash_.constEnd(); ++p)
      ret.append(p.value());
  return ret;
}

const AnnotationList&
AnnotationGraphicsView::annotationsAtPos(qint64 pos) const
{
  static const AnnotationList null;

  auto p = hash_.find(pos);
  return p == hash_.end() ? null : p.value();
  //if (annots_.contains(pos)) {
  //  QList<AnnotationGraphicsItem*> *l = annots_[pos];
  //  if (l && !l->empty())
  //    foreach (AnnotationGraphicsItem *item, *l)
  //      ret.append(item->annotation());
  //}
  //return ret;
}

// - View tracking -

void
AnnotationGraphicsView::updateGeometry()
{
  DOUT("enter: fullScreen =" << fullScreen_);
  updatePos();
  updateSize();
  DOUT("exit");
}

void
AnnotationGraphicsView::updateSize()
{
  bool update = false;
  if (fullScreen_ && fullScreenView_) {
    resize(fullScreenView_->size());
    update = true;
  } else if (trackedWindow_) {
#ifdef Q_OS_WIN
    QRect r = QtWin::getWindowRect(trackedWindow_);
    if (r.isEmpty()) {
      //if (!QtWin::isWindowValid(trackedWindow_)) {
      setTrackedWindow(nullptr);
      DOUT("tracked window destroyed");
      emit trackedWindowDestroyed();
    } else if (r.topLeft() != QTWIN_INVALID_POS && r.size() != size()) {
      resize(r.size());
      update = true;
    }
#endif // Q_OS_WIN
  } else if (!hub_->isSignalTokenMode() || hub_->isNormalPlayerMode()) {
    // FIXME: 10/24/2011: Screen not working orz
//#ifdef Q_OS_MAC
//    if (fullScreenView_)
//      resize(fullScreenView_->size());
//#else
    resize(videoView_->size());
    update = true;
//#endif // Q_OS_MAC

  }
  //else if (hub_->isSignalTokenMode() && fullScreenView_) {
  //  resize(fullScreenView_->size());
  //  update = true;
  //}

  if (update) {
    setSceneRect(0, 0, width(), height());
    emit sizeChanged(size());
  }
  updateMaxItemCount();
}

void
AnnotationGraphicsView::updatePos()
{
  if (fullScreen_ && fullScreenView_) {
    move(fullScreenView_->pos());
    emit posChanged();
  } else if (trackedWindow_) {
#ifdef Q_OS_WIN
    QRect r = QtWin::getWindowRect(trackedWindow_);
    if (r.isEmpty()) {
      //if (!QtWin::isWindowValid(trackedWindow_))
      setTrackedWindow(nullptr);
      DOUT("tracked window destroyed");
      emit trackedWindowDestroyed();
    } else {
      QPoint newPos = r.topLeft();
      if (newPos == QTWIN_INVALID_POS) {
        //if (QtWin::isWindowMinimized(trackedWindow_))
        //  TODO: minimize();
      } else
        moveToGlobalPos(newPos);
      return;
    }
#endif // Q_OS_WIN
  } else {

    QPoint newPos;
#ifdef Q_OS_MAC
    // Since videoView_ could change its pos() while playing video, use its parent widget instead.
    if (videoView_->parentWidget())
      newPos = videoView_->parentWidget()->mapToGlobal(QPoint());
    else
#endif // Q_QW_MAC
#ifdef Q_OS_LINUX
    // FIXME: Hot fix for Ubuntu Unity top bar. TO BE REMOVED!
    if (fullScreen_ && fullScreenView_ && hub_->isEmbeddedPlayerMode())
      newPos = fullScreenView_->mapToGlobal(QPoint());
    else
#endif // Q_OS_LINUX
    newPos = videoView_->mapToGlobal(QPoint());
    moveToGlobalPos(newPos);

  }
  //else if (hub_->isSignalTokenMode() && fullScreenView_) {
  //  move(fullScreenView_->pos());
  //  emit posChanged();
  //}
}

QPoint
AnnotationGraphicsView::fromGlobal(const QPoint &globalPos) const
{
  return
#ifdef Q_OS_MAC
      mapFromGlobal(globalPos) - mapFromGlobal(QPoint())
#else
      // Currently only work on Windows
      frameGeometry().topLeft() + pos() + // relative position
      globalPos - mapToGlobal(pos()) // absolute distance
#endif // Q_OS_MAC
  ;
}

void
AnnotationGraphicsView::moveToGlobalPos(const QPoint &globalPos)
{
  QPoint newPos = fromGlobal(globalPos);
  if (newPos != pos()) {
    move(newPos);
    emit posChanged();
  }
}

void
AnnotationGraphicsView::setVisible(bool visible)
{
  if (visible == isVisible())
    return;

  if (visible)
    updateGeometry();
  Base::setVisible(visible);

  updateTrackingTimer();

  emit visibleChanged(visible);
}

// - Player connections -

void
AnnotationGraphicsView::setPlaybackEnabled(bool enabled)
{
  if (playbackEnabled_ != enabled) {
    playbackEnabled_= enabled;
    emit playbackEnabledChanged(enabled);
  }
}

// - Control -

void
AnnotationGraphicsView::expelItems(const QPoint &center)
{
  enum { width = 600, height = 400 };
  QRect r(0, 0, width, height);
  r.moveCenter(center);
  expelItems(center, r, Qt::IntersectsItemBoundingRect);
}

void
AnnotationGraphicsView::expelItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode))
    static_cast<AnnotationGraphicsItem *>(item)
      ->escapeFrom(center);
}

void
AnnotationGraphicsView::expelAllItems(const QPoint &center)
{
  foreach (QGraphicsItem *item, items())
    static_cast<AnnotationGraphicsItem *>(item)
      ->escapeFrom(center);
}

void
AnnotationGraphicsView::attractItems(const QPoint &center)
{
  enum { width = 800, height = 800 };
  QRect r(0, 0, width, height);
  r.moveCenter(center);
  attractItems(center, r, Qt::IntersectsItemBoundingRect);
}

void
AnnotationGraphicsView::attractItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode))
    static_cast<AnnotationGraphicsItem *>(item)
      ->rushTo(center);
}

void
AnnotationGraphicsView::attractAllItems(const QPoint &center)
{
  foreach (QGraphicsItem *item, items())
    static_cast<AnnotationGraphicsItem *>(item)
      ->rushTo(center);
}

void
AnnotationGraphicsView::pause()
{
  if (!paused_) {
    paused_ = true;
    scheduler_->pause();
  }
  emit paused();
}

void
AnnotationGraphicsView::resume()
{
  if (paused_) {
    paused_ = false;
    scheduler_->resume();
  }
  emit resumed();
}

void
AnnotationGraphicsView::selectItem(AnnotationGraphicsItem *item, bool detail)
{
  Q_ASSERT(item);
  if (!item)
    return;
  qint64 uid = item->annotation().userId();
  if (uid && hub_->isMediaTokenMode())
    emit selectedUserIds(QList<qint64>() << uid);
  if (detail && uid)
    emit selectedUserId(uid);
}

void
AnnotationGraphicsView::releaseItem(AnnotationGraphicsItem *item)
{ pool_->release(item); }

void
AnnotationGraphicsView::pauseItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  if (item) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    a->pause();
    selectItem(a);
  }
}

void
AnnotationGraphicsView::resumeItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  if (item)
   static_cast<AnnotationGraphicsItem *>(item)
     ->resume();
}

void
AnnotationGraphicsView::removeItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  if (item)
    static_cast<AnnotationGraphicsItem *>(item)
      ->disappear();
}

void
AnnotationGraphicsView::pauseItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  QList<qint64> uids;
  foreach (QGraphicsItem *item, items(rect, mode)) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    a->pause();
    qint64 uid = a->annotation().userId();
    if (uid)
      uids.append(uid);
  }

  if (!uids.isEmpty()) {
    boost::unique(uids);
    emit selectedUserIds(uids);
  }
}

void
AnnotationGraphicsView::resumeItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode))
    static_cast<AnnotationGraphicsItem *>(item)
      ->resume();
}

void
AnnotationGraphicsView::removeItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode))
    static_cast<AnnotationGraphicsItem *>(item)
      ->disappear();
}

void
AnnotationGraphicsView::pauseItems(const QPoint &pos)
{
  QList<qint64> uids;
  foreach (QGraphicsItem *item, items(pos)) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    a->pause();
    qint64 uid = a->annotation().userId();
    if (uid)
      uids.append(uid);
  }
  if (!uids.isEmpty()) {
    boost::unique(uids);
    emit selectedUserIds(uids);
  }
}

void
AnnotationGraphicsView::resumeItems(const QPoint &pos)
{
  foreach (QGraphicsItem *item, items(pos))
    static_cast<AnnotationGraphicsItem *>(item)
      ->resume();
}

void
AnnotationGraphicsView::removeItems(const QPoint &pos)
{
  foreach (QGraphicsItem *item, items(pos))
    static_cast<AnnotationGraphicsItem *>(item)
      ->disappear();
}

void
AnnotationGraphicsView::removeAllItems()
{
  foreach (QGraphicsItem *item, items())
    static_cast<AnnotationGraphicsItem *>(item)
      ->disappear();
  scheduler_->clear();
}

bool
AnnotationGraphicsView::hasPausedItems() const
{
  foreach (QGraphicsItem *item, items()) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    if (a->isPaused())
      return true;
  }
  return false;
}

void
AnnotationGraphicsView::scalePausedItems(qreal scale)
{
  foreach (QGraphicsItem *item, items()) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    if (a->isPaused())
      a->setScale(a->scale() * scale);
  }
}

void
AnnotationGraphicsView::rotatePausedItems(qreal angle)
{
  foreach (QGraphicsItem *item, items()) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    if (a->isPaused())
      a->setRotation(a->rotation() + angle);
  }
}

// - Annotations -

//qint64
//AnnotationGraphicsView::playTime() const
//{ return playTime_; }
//
//void
//AnnotationGraphicsView::setPlayTime(qint64 time)
//{ playTime_ = time; }

void
AnnotationGraphicsView::updateCurrentTime()
{
  if (!player_->hasMedia()
      || !playbackEnabled_)
    return;

  qint64 msecs = player_->time();
  if (msecs < 0)
    return;

  // jichi 7/27/2011: Expect this to be a problem
  qint64 secs = msecs / 1000;
  if (secs != currentTime_) { // currentTime_ is used to prevent the same seconds being double displayed
    currentTime_ = secs;
    showAnnotationsAtPos(currentTime_ * 1000);
  }
}

void
AnnotationGraphicsView::invalidateAnnotations()
{
  removeAnnotations();
  //if (player_->hasMedia()) {
  //  qint64 msecs = player_->mediaLength();
  //  if (msecs < 0)
  //    return;
  //  int secs = ::msecs2secs(msecs);
  //  annots_.resize(secs + 1); // +1 to make sure the last seconds can be held
  //}

  filteredAnnotationIds_.clear();

  currentTime_ = -1;
  emit annotationPosChanged(0);
}

void
AnnotationGraphicsView::addAnnotations(const AnnotationList &annots)
{
  DOUT("enter: size =" << annots.size());
  if (!annots.empty())
    foreach (const Annotation &a, annots)
      addAnnotation(a);
  DOUT("exit");
}

void
AnnotationGraphicsView::addAnnotation(const Annotation &annot, qint64 delaysecs)
{
  //DOUT("enter: aid =" << annot.id() << ", pos =" << annot.pos());

  qint64 pos = annot.pos();
  if (!hub_->isSignalTokenMode())
    pos = qRound64(pos / 1000.0); // msecs => secs, cluster media annotation by seconds

  //QList<AnnotationGraphicsItem*> *l = annots_[pos];
  //if (!l)
  //  l = annots_[pos] = new QList<AnnotationGraphicsItem*>;
  //l->append(item);
  hash_[pos].append(annot);

  if (isAnnotationBlocked(annot))
    return;

  bool show = false;
  if (delaysecs == LLONG_MAX)
    show = true;
  else if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    qint64 secsdiff =  player_->time() / 1000 - pos;
    if (secsdiff >= 0 && secsdiff <= delaysecs)
      show = true;
  }

  if (show)
    showAnnotation(annot);

  emit annotationAdded(annot);
  //DOUT("exit");
}

void
AnnotationGraphicsView::addAndShowAnnotation(const Annotation &annot)
{ addAnnotation(annot, LLONG_MAX); }

void
AnnotationGraphicsView::updateMaxItemCount()
{ maxItemCount_ = qMax<int>(ReservedItemCount, 30 + 10 * width() * height() / (800*600)); }

void
AnnotationGraphicsView::showAnnotation(const Annotation &annot, bool showMeta)
{
  if (itemCountLimited_ &&
      pool_->size() >= maxItemCount_ &&
      annot.hasUserId() && annot.userId() != userId_ && !annot.isSubtitle()) {
    //DOUT("too many annotations, skip");
    emit annotationSkipped();
    qDebug() << "AnnotationGraphicsView::showAnnotation: too many annotations, skip";
    return;
  }

  if (!isAnnotationBlocked(annot)) {
    AnnotationGraphicsItem *item = pool_->allocate();
    item->reset();
    item->setMetaVisible(metaVisible_ && showMeta);
    item->setAvatarVisible(AnnotationSettings::globalSettings()->isAvatarVisible());
    item->setAnnotation(annot);
    if (!isItemBlocked(item))
      item->showMe();
    if (item->isSubtitle())
      emit subtitleAdded(item->text());
    else
      emit annotationAdded(item->text()); // non-subtitle added

    //qDebug()<< isVisible() << size() << pos() << trackedWindow_ << trackingTimer_->isActive();
  }
}

void
AnnotationGraphicsView::showAnnotationOnce(const Annotation &annot)
{
  showAnnotation(annot);
  if (annot.hasId())
    filteredAnnotationIds_.append(annot.id());
}

// - Queries -

int
AnnotationGraphicsView::itemsCount() const
{
  int ret = 0;
  if (!hash_.isEmpty())
    for (auto p = hash_.constBegin(); p != hash_.constEnd(); ++p)
      ret += p.value().size();

  return ret;
}

int
AnnotationGraphicsView::itemsCount(int time) const
{
  //int ret = 0;
  //auto it = annots_.find(time);
  //if (it != annots_.end()) {
  //  auto l = *it;
  //  if (l)
  //    ret = l->size();
  //}
  //return ret;

  return annotationsAtPos(time).size();
}

int
AnnotationGraphicsView::itemsCount(int from, int to) const
{
  int ret = 0;
  for (int time = from; time < to; time++)
    ret += itemsCount(time);
  return ret;
}

// - Events -

void
AnnotationGraphicsView::sendContextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  QGraphicsItem *item = itemAt(mapFromGlobal(event->globalPos()));
  if (item)
    static_cast<AnnotationGraphicsItem *>(item)
      ->contextMenuEvent(event);
}

void
AnnotationGraphicsView::sendMouseDoubleClickEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  dragging_ = false;
  QGraphicsItem *item = itemAt(mapFromGlobal(event->globalPos()));
  if (item)
    static_cast<AnnotationGraphicsItem *>(item)
      ->mouseDoubleClickEvent(event);
}

void
AnnotationGraphicsView::sendMousePressEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  dragging_ = false;
  foreach (QGraphicsItem *item, items(mapFromGlobal(event->globalPos()))) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    a->mousePressEvent(event);
    if (a->isDragging())
      dragging_ = true;
  }
  if (dragging_)
    Application::globalInstance()->setCursor(Qt::ClosedHandCursor);
}

void
AnnotationGraphicsView::sendMouseReleaseEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  bool dragging = false;
  foreach (QGraphicsItem *item, items(mapFromGlobal(event->globalPos()))) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    if (dragging_)
      dragging = a->isDragging();
    a->mouseReleaseEvent(event);
  }
  if (dragging)
    Application::globalInstance()->setCursor(Qt::OpenHandCursor);
  if (dragging_ && !dragging)
    foreach (QGraphicsItem *item, items()) {
      auto a = static_cast<AnnotationGraphicsItem *>(item);
      if (a->isDragging()) {
        a->mouseReleaseEvent(event);
        dragging = true;
      }
    }
  dragging_ = false;
}

void
AnnotationGraphicsView::sendMouseMoveEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  QList<QGraphicsItem *> l = items(mapFromGlobal(event->globalPos()));
  bool dragging = false;
  bool paused = false;
  foreach (QGraphicsItem *item, l) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    a->mouseMoveEvent(event);
    if (dragging_ && a->isDragging())
      dragging = true;
    paused = a->isPaused();
  }

  if (dragging_ && !dragging)
    foreach (QGraphicsItem *item, items()) {
      auto a = static_cast<AnnotationGraphicsItem *>(item);
      if (a->isDragging()) {
        a->mouseMoveEvent(event);
        dragging = true;
      }
    }

  Application::globalInstance()->setCursor(
       dragging ? Qt::ClosedHandCursor :
         paused ? Qt::OpenHandCursor :
    l.isEmpty() ? Qt::ArrowCursor :
                  Qt::PointingHandCursor
  );
}

// - Message mode -

void
AnnotationGraphicsView::showAnnotationsInContext(qint64 h1, qint64 h4)
{
  DOUT("enter: h1 =" << h1 << ", h4 =" << h4);

  emit annotationPosChanged(h1);

  if (hub_->isSignalTokenMode()) {
    emit removeSubtitlesRequested();
    scheduler_->clearSubtitles();
  } else {
    //pos /= 1000;
    //pos -= offset_;
    Q_ASSERT(0);
  }

  //if (annots_.contains(pos)) {
  //  DOUT("found annotations at pos =" << pos);
  //  QList<AnnotationGraphicsItem*> *l = annots_[pos];
  //  if (l && !l->empty())
  //    foreach (AnnotationGraphicsItem *item, *l)
  //     if (item && !isItemBlocked(item))
  //       item->showMe();
  //}
  //
  //foreach (Annotation a, l)
  //  showAnnotation(a);
  const AnnotationList &l1 = annotationsAtPos(h1);
  if (!l1.isEmpty())
    foreach (const Annotation &a, l1)
      showAnnotation(a);

  if (h4 && h4 != h1) {
    const AnnotationList &l4 = annotationsAtPos(h4);
    if (!l4.isEmpty())
      foreach (const Annotation &a, l4)
        showAnnotation(a);
  }

  DOUT("exit");
}

void
AnnotationGraphicsView::showAnnotationsAtPos(qint64 pos)
{
  DOUT("enter: pos =" << pos);

  emit annotationPosChanged(pos);

  if (hub_->isSignalTokenMode()) {
    emit removeSubtitlesRequested();
    scheduler_->clearSubtitles();
  } else {
    //pos = qRound64(pos / 1000.0); // msecs => secs
    pos /= 1000;
    pos -= offset_;
  }

  //if (annots_.contains(pos)) {
  //  DOUT("found annotations at pos =" << pos);
  //  QList<AnnotationGraphicsItem*> *l = annots_[pos];
  //  if (l && !l->empty())
  //    foreach (AnnotationGraphicsItem *item, *l)
  //     if (item && !isItemBlocked(item))
  //       item->showMe();
  //}
  //
  //foreach (Annotation a, l)
  //  showAnnotation(a);
  const AnnotationList &l = annotationsAtPos(pos);
  if (!l.isEmpty())
    for (auto p = l.constBegin(); p != l.constEnd(); ++p)
      showAnnotation(*p);

  DOUT("exit");
}

void
AnnotationGraphicsView::updateAnnotationText(const QString &text)
{
  if (text.isEmpty())
    return;
  qint64 id = editor()->id();
  if (id) {
    updateAnnotationTextWithId(text, id);
    emit annotationTextUpdatedWithId(text, id);
  } else
    emit annotationTextSubmitted(text);
}

void
AnnotationGraphicsView::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  AnnotationGraphicsItem *item = itemWithId(id);
  if (item) {
    item->annotation().setText(text);
    item->annotation().setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
    item->updateText();
  }

  Annotation *a = annotationWithId(id);
  if (a) {
    a->setText(text);
    a->setUpdateTime(QDateTime::currentMSecsSinceEpoch());
  }
}

AnnotationGraphicsItem*
AnnotationGraphicsView::itemWithId(qint64 id) const
{
  //if (!annots_.empty())
  //  foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
  //    if (l && !l->empty())
  //      foreach (AnnotationGraphicsItem *item, *l)
  //        if (item && item->annotation().id() == id)
  //          return item;
  //return 0;
  foreach (QGraphicsItem *item, scene()->items()) {
    auto a = static_cast<AnnotationGraphicsItem *>(item);
    if (a->annotation().id() == id)
      return a;
  }
  return 0;
}

Annotation*
AnnotationGraphicsView::annotationWithId(qint64 id) const
{
  for (auto h = hash_.begin(); h != hash_.end(); ++h) {
    const AnnotationList &l = h.value();
    for (auto p = l.begin(); p != l.end(); ++p)
      if (p->id() == id)
        return const_cast<Annotation *>(&*p);
  }
  return 0;
}

bool
AnnotationGraphicsView::isAnnotationBlocked(const Annotation &a) const
{
  return a.isHidden() ||
      a.hasId() && filteredAnnotationIds_.contains(a.id()) ||
      filter_ && filter_->filter(a) ;
}

bool
AnnotationGraphicsView::isItemBlocked(const AnnotationGraphicsItem *item) const
{
  return !item ||
      !subtitleVisible_ && item->isSubtitle() ||
      !nonSubtitleVisible_ && !item->isSubtitle();
}

// - Tracking -

void
AnnotationGraphicsView::setTrackedWindow(WId hwnd)
{
  DOUT("enter: hwnd =" << hwnd);
  if (hwnd && (
      hwnd == videoView_->winId()
#ifdef WITH_WIN_HOOK
      ||  videoView_->containsWindow(hwnd)
#endif // WITH_WIN_HOOK
#ifdef Q_OS_WIN
      || QtWin::getChildWindows(videoView_->winId()).contains(hwnd)
      || QtWin::getWindowProcessId(hwnd) == QCoreApplication::applicationPid()
#endif // Q_OS_WIN
      ))
    hwnd = 0;

  if (trackedWindow_ != hwnd) {
     trackedWindow_ = hwnd;
     updateTrackingTimer();
     DOUT("tracked window changed");
     emit trackedWindowChanged(trackedWindow_);
  }
#ifdef WITH_WIN_MOUSEHOOK
  if (trackedWindow_)
    MouseHook::globalInstance()->start();
  else
    MouseHook::globalInstance()->stop();
#endif // WITH_WIN_MOUSEHOOK
  updateGeometry();
  DOUT("exit");
}

void
AnnotationGraphicsView::updateTrackingTimer()
{
  DOUT("enter: visible =" << isVisible() << ", fullScreen =" << fullScreen_ << ", trackedWindow_ =" << trackedWindow_);
  if (!fullScreen_ && trackedWindow_) {
    DOUT("start tracking");
    startTracking();
  } else {
    DOUT("stop tracking");
    stopTracking();
  }
  DOUT("exit");
}

void
AnnotationGraphicsView::startTracking()
{
  if (!trackingTimer_->isActive())
    trackingTimer_->start();
}

void
AnnotationGraphicsView::stopTracking()
{
  if (trackingTimer_->isActive())
    trackingTimer_->stop();
}

QRect
AnnotationGraphicsView::globalRect() const
{
  QPoint topLeft = mapToGlobal(QPoint());
  return QRect(topLeft, size());
}

// - Live mode -

bool
AnnotationGraphicsView::isStarted() const
{ return timer_->isActive(); }

void
AnnotationGraphicsView::start()
{ timer_->start(); }

void
AnnotationGraphicsView::stop()
{
  if (timer_->isActive())
    timer_->stop();
}

void
AnnotationGraphicsView::tick()
{
  if (playbackEnabled_)
    showAnnotationsAtPos(currentTime_ * 1000);
  currentTime_++;
}

void
AnnotationGraphicsView::appendAnnotations(const AnnotationList &annots)
{
  if (annots.isEmpty())
    return;
  auto p = annots.constBegin();

  int n = annots.size();
  int k = interval_ / 1000;
  if (k <= 0)
    k = 1;
  int part = n / k;
  if (part)
    for (int i = 0; i < k; i++) {
      AnnotationList &l = hash_[currentTime_ + i];
      for (int j = 0; j < part; j++)
        l.append(*p++);
    }

  AnnotationList &l = hash_[currentTime_+ k - 1];
  while (p != annots.constEnd())
    l.append(*p++);

  foreach (const Annotation &a, annots)
    emit annotationAdded(a);
}

void
AnnotationGraphicsView::setScale(qreal value)
{
 // qreal r = value / scale_;
  //Base::scale(r, r);
  if (scale_ != value)
    emit scaleChanged(scale_ = value);
}

void
AnnotationGraphicsView::updateScale()
{
  setScale(hub_->isFullScreenWindowMode() ?
    AnnotationSettings::globalSettings()->fullscreenScale() :
    AnnotationSettings::globalSettings()->scale()
  );
}

void
AnnotationGraphicsView::setRotation(qreal value)
{
  bool paused = paused_;
  if (!paused)
    pause(); // lock current items to prevent segmentation fault

  qreal delta = value - rotation_;
  Base::rotate(delta);

  if (!paused)
    QTimer::singleShot(0, this, SLOT(resume()));

  rotation_ = value;
  emit rotationChanged(rotation_);
}


// - Search/Translate -

void
AnnotationGraphicsView::searchText(const QString &text, int engine)
{
  emit message(tr("search") + ": " + HTML_SS_OPEN(color:orange) + text + HTML_SS_CLOSE());
  emit searchRequested(engine,text);
}

void
AnnotationGraphicsView::translateText(const QString &text, int lang)
{
  emit message(tr("translate") + ": " + HTML_SS_OPEN(color:orange) + text + HTML_SS_CLOSE());
  emit translateRequested(text, lang);
}

void
AnnotationGraphicsView::showTraditionalChinese(const QString &text)
{ emit traditionalChineseRequested(text); }

// EOF
