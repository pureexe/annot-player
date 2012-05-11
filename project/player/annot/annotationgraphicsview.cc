// annotationgraphicsview.cc
// 7/14/2011

#include "annotationgraphicsview.h"
#include "annotationgraphicsitem.h"
#include "annotationgraphicsitempool.h"
#include "annotationeditor.h"
#include "annotationfilter.h"
#include "stylesheet.h"
#include "signalhub.h"
#include "videoview.h"
#include "logger.h"
#include "global.h"
#include "module/player/player.h"
#include "module/qtext/algorithm.h"
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#ifdef WITH_WIN_MOUSEHOOK
#  include "win/mousehook/mousehook.h"
#endif // WITH_WIN_MOUSEHOOK
#include <QtGui>
#include <boost/typeof/typeof.hpp>
#include <climits>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

//#define DEBUG "annotationgraphicsview"
#include "module/debug/debug.h"

using namespace AnnotCloud;
using namespace Logger;

#define MAX_SUBTITLE_HISTORY    30
#define TIMER_INTERVAL        1000 // 1 second

// - Construction -

AnnotationGraphicsView::AnnotationGraphicsView(
  SignalHub *hub,
  Player *player,
  VideoView *view,
  QWidget *parent)
  : Base(parent), videoView_(view), fullScreenView_(0), trackedWindow_(0), editor_(0),
    hub_(hub), player_(player), filter_(0), renderHint_(DefaultRenderHint), active_(false), paused_(false), fullScreen_(false),
    subtitleVisible_(true), nonSubtitleVisible_(true),
    currentTime_(-1), offset_(0), interval_(TIMER_INTERVAL), userId_(0), playbackEnabled_(true), subtitlePosition_(AP_Bottom),
    scale_(1.0), rotation_(0),
    hoveredItemPaused_(false), hoveredItemResumed_(false), hoveredItemRemoved_(false), nearbyItemExpelled_(false), nearbyItemAttracted_(false),
    itemVisible_(true)
{
  Q_ASSERT(hub_);
  Q_ASSERT(player_);
  Q_ASSERT(videoView_);

  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(SS_GRAPHICSVIEW);

  //setAttribute(Qt::WA_TransparentForMouseEvents);
  //setFocusPolicy(Qt::NoFocus);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setResizeAnchor(NoAnchor);
  setTransformationAnchor(NoAnchor);
  setAlignment(Qt::AlignRight | Qt::AlignTop);

  QGraphicsScene *scene = new QGraphicsScene(this);
  setScene(scene);

  pool_ = new AnnotationGraphicsItemPool(this, hub_, this);
  pool_->reserve(40);

  timer_ = new QTimer(this);
  timer_->setInterval(TIMER_INTERVAL);
  connect(timer_, SIGNAL(timeout()), SLOT(tick()));

  trackingTimer_ = new QTimer(this);
  trackingTimer_->setInterval(G_TRACKING_INTERVAL);
  connect(trackingTimer_, SIGNAL(timeout()), SLOT(updateGeometry()));

//#ifdef Q_WS_WIN
  setRenderHints(
    //QPainter::Antialiasing
    QPainter::TextAntialiasing
    //QPainter::SmoothPixmapTransform // would cost so much CPU
  );
//#endif // Q_WS_WIN

  connect(this, SIGNAL(offsetChanged(qint64)), SLOT(removeAllItems()));

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
    BOOST_AUTO(self, const_cast<Self *>(this));
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
    for (BOOST_AUTO(h, hash_.begin()); h != hash_.end() && !found; ++h) {
      AnnotationList &l = h.value();
      for (BOOST_AUTO(p, l.begin()); p != l.end() && !found; ++p)
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
    for (BOOST_AUTO(p, hash_.begin()); p != hash_.end(); ++p)
      ret.append(p.value());
  return ret;
}

const AnnotationList&
AnnotationGraphicsView::annotationsAtPos(qint64 pos) const
{
  static const AnnotationList null;

  BOOST_AUTO(p, hash_.find(pos));
  if (p!= hash_.end())
    return p.value();
  else
    return null;
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
  updatePos();
  updateSize();
}

void
AnnotationGraphicsView::updateSize()
{
  bool update = false;
  if (fullScreen_ && fullScreenView_) {
    resize(fullScreenView_->size());
    update = true;
  } else if (trackedWindow_) {
#ifdef Q_WS_WIN
    QRect r = QtWin::getWindowRect(trackedWindow_);
    if (r.isNull()) {
      if (!QtWin::isValidWindow(trackedWindow_)) {
        setTrackedWindow(0);
        emit trackedWindowDestroyed();
      }
    } else if (size() != r.size()) {
      resize(r.size());
      update = true;
    }
#endif // Q_WS_WIN
  } else if (!hub_->isSignalTokenMode() || hub_->isNormalPlayerMode()) {
    // FIXME: 10/24/2011: Screen not working orz
//#ifdef Q_WS_MAC
//    if (fullScreenView_)
//      resize(fullScreenView_->size());
//#else
    resize(videoView_->size());
    update = true;
//#endif // Q_WS_MAC

  } else if (hub_->isSignalTokenMode() && fullScreenView_) {
    resize(fullScreenView_->size());
    update = true;
  }

  if (update) {
    setSceneRect(0, 0, width(), height());
    emit sizeChanged();
  }
}

void
AnnotationGraphicsView::updatePos()
{
  if (fullScreen_ && fullScreenView_) {
    move(fullScreenView_->pos());
    emit posChanged();
  } else if (trackedWindow_) {
#ifdef Q_WS_WIN
    QRect r = QtWin::getWindowRect(trackedWindow_);
    if (r.isNull()) {
      if (!QtWin::isValidWindow(trackedWindow_))
        setTrackedWindow(0);
    } else {
      QPoint newPos = r.topLeft();
      moveToGlobalPos(newPos);
      return;
    }
#endif // Q_WS_WIN
  } else if (!hub_->isSignalTokenMode() || hub_->isNormalPlayerMode()) {

    QPoint newPos;
#ifdef Q_WS_MAC
    // Since videoView_ could change its pos() while playing video, use its parent widget instead.
    if (videoView_->parentWidget())
      newPos = videoView_->parentWidget()->mapToGlobal(QPoint());
    else
#endif // Q_QW_MAC
#ifdef Q_WS_X11
    // FIXME: Hot fix for Ubuntu Unity top bar. TO BE REMOVED!
    if (fullScreen_ && fullScreenView_ && hub_->isEmbeddedPlayerMode())
      newPos = fullScreenView_->mapToGlobal(QPoint());
    else
#endif // Q_WS_X11
    newPos = videoView_->mapToGlobal(QPoint());
    moveToGlobalPos(newPos);

  } else if (hub_->isSignalTokenMode() && fullScreenView_) {
    move(fullScreenView_->pos());
    emit posChanged();
  }
}

QPoint
AnnotationGraphicsView::fromGlobal(const QPoint &globalPos) const
{
  return
#ifdef Q_WS_MAC
      mapFromGlobal(globalPos) - mapFromGlobal(QPoint())
#else
      // Currently only work on Windows
      frameGeometry().topLeft() + pos() + // relative position
      globalPos - mapToGlobal(pos()) // absolute distance
#endif // Q_WS_MAC
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

  setActive(visible);
  if (visible)
    updateGeometry();
  Base::setVisible(visible);

  updateTrackingTimer();

  emit visibleChanged(visible);
}

// - Player connections -

#define SET_PLAYER_CONNECTIONS(_connect) \
  void \
  AnnotationGraphicsView::_connect##Player() \
  { \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(invalidateAnnotations())); \
    _connect(player_, SIGNAL(timeChanged()), this, SLOT(updateCurrentTime())); \
    _connect(player_, SIGNAL(paused()), this, SLOT(pause())); \
    _connect(player_, SIGNAL(playing()), this, SLOT(resume())); \
    _connect(player_, SIGNAL(stopped()), this, SLOT(resume())); \
    _connect(player_, SIGNAL(stopped()), this, SIGNAL(removeItemRequested())); \
    _connect(player_, SIGNAL(stopped()), this, SLOT(removeAllItems())); \
  }

  SET_PLAYER_CONNECTIONS(connect)
  SET_PLAYER_CONNECTIONS(disconnect)
#undef PLAYER_CONNECTIONS

#define SET_HUB_CONNECTIONS(_connect) \
  void \
  AnnotationGraphicsView::_connect##Hub() \
  { \
    _connect(hub_, SIGNAL(paused()), this, SLOT(pause())); \
    _connect(hub_, SIGNAL(played()), this, SLOT(resume())); \
    _connect(hub_, SIGNAL(stopped()), this, SLOT(resume())); \
    _connect(hub_, SIGNAL(stopped()), this, SIGNAL(removeItemRequested())); \
    _connect(hub_, SIGNAL(stopped()), this, SLOT(removeAllItems())); \
  }

  SET_HUB_CONNECTIONS(connect)
  SET_HUB_CONNECTIONS(disconnect)
#undef HUB_CONNECTIONS

void
AnnotationGraphicsView::setActive(bool active)
{
  if (active) {
    if (!active_) {
      connectPlayer();
      connectHub();
      active_ = true;
    }
  } else if (active_) {
    disconnectHub();
    active_ = false;
  }
}

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
  foreach (QGraphicsItem *item, items(rect, mode)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->escapeFrom(center);
  }
}

void
AnnotationGraphicsView::expelAllItems(const QPoint &center)
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->escapeFrom(center);
  }
}

void
AnnotationGraphicsView::attractItems(const QPoint &center)
{
  enum { width = 800, height = 600 };
  QRect r(0, 0, width, height);
  r.moveCenter(center);
  attractItems(center, r, Qt::IntersectsItemBoundingRect);
}

void
AnnotationGraphicsView::attractItems(const QPoint &center, const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->rushTo(center);
  }
}

void
AnnotationGraphicsView::attractAllItems(const QPoint &center)
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->rushTo(center);
  }
}

void
AnnotationGraphicsView::pause()
{
  if (!paused_)
    paused_ = true;
  emit paused();
}

void
AnnotationGraphicsView::resume()
{
  if (paused_)
    paused_ = false;
  emit resumed();
}

void
AnnotationGraphicsView::selectItem(AnnotationGraphicsItem *item, bool detail)
{
  qint64 uid = item->annotation().userId();
  if (uid)
    emit selectedUserIds(QList<qint64>() << uid);
  if (detail)
    emit selectedUserId(uid);
}

void
AnnotationGraphicsView::releaseItem(AnnotationGraphicsItem *item)
{ pool_->release(item); }

void
AnnotationGraphicsView::pauseItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
  if (a) {
    a->pause();
    selectItem(a);
  }
}

void
AnnotationGraphicsView::resumeItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
  if (a)
    a->pause();
}

void
AnnotationGraphicsView::removeItemAt(const QPoint &pos)
{
  QGraphicsItem *item = itemAt(pos);
  BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
  if (a)
    a->disappear();
}

void
AnnotationGraphicsView::pauseItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  QList<qint64> uids;
  foreach (QGraphicsItem *item, items(rect, mode)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a) {
      a->pause();
      qint64 uid = a->annotation().userId();
      if (uid)
        uids.append(uid);
    }
  }

  if (!uids.isEmpty())
    emit selectedUserIds(QtExt::uniqueList(uids));
}

void
AnnotationGraphicsView::resumeItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->resume();
  }
}

void
AnnotationGraphicsView::removeItems(const QRect &rect, Qt::ItemSelectionMode mode)
{
  foreach (QGraphicsItem *item, items(rect, mode)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->disappear();
  }
}

void
AnnotationGraphicsView::pauseItems(const QPoint &pos)
{
  QList<qint64> uids;
  foreach (QGraphicsItem *item, items(pos)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a) {
      a->pause();
      qint64 uid = a->annotation().userId();
      if (uid)
        uids.append(uid);
    }
  }
  if (!uids.isEmpty())
    emit selectedUserIds(QtExt::uniqueList(uids));
}

void
AnnotationGraphicsView::resumeItems(const QPoint &pos)
{
  foreach (QGraphicsItem *item, items(pos)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->resume();
  }
}

void
AnnotationGraphicsView::removeItems(const QPoint &pos)
{
  foreach (QGraphicsItem *item, items(pos)) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->disappear();
  }
}

void
AnnotationGraphicsView::removeAllItems()
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a)
      a->disappear();
  }
}

bool
AnnotationGraphicsView::hasPausedItems() const
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a && a->isPaused())
      return true;
  }
  return false;
}

void
AnnotationGraphicsView::scalePausedItems(qreal scale)
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a && a->isPaused())
      a->setScale(a->scale() * scale);
  }
}

void
AnnotationGraphicsView::rotatePausedItems(qreal angle)
{
  foreach (QGraphicsItem *item, items()) {
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a && a->isPaused())
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
  DOUT("enter: aid =" << annot.id() << ", pos =" << annot.pos());

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
  DOUT("exit");
}

void
AnnotationGraphicsView::addAndShowAnnotation(const Annotation &annot)
{ addAnnotation(annot, LLONG_MAX); }

void
AnnotationGraphicsView::showAnnotation(const Annotation &annot)
{
  if (!isAnnotationBlocked(annot)) {
    AnnotationGraphicsItem *item = pool_->allocate();
    item->setAnnotation(annot);
    if (!isItemBlocked(item))
      item->showMe();
    if (item->isSubtitle())
      emit subtitleAdded(item->richText());
    else
      emit annotationAdded(item->richText()); // non-subtitle added
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
    for (BOOST_AUTO(p, hash_.begin()); p != hash_.end(); ++p)
      ret += p.value().size();

  return ret;
}

int
AnnotationGraphicsView::itemsCount(int time) const
{
  //int ret = 0;
  //BOOST_AUTO(it, annots_.find(time));
  //if (it != annots_.end()) {
  //  BOOST_AUTO(l, *it);
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

#define SEND_EVENT(_sender, _receiver, _event) \
  void \
  AnnotationGraphicsView::_sender(_event *event) \
  { \
    DOUT("enter"); \
    if (event) { \
      QGraphicsItem *item = itemAt(mapFromGlobal(event->globalPos())); \
      BOOST_AUTO(annot, dynamic_cast<AnnotationGraphicsItem *>(item)); \
      if (annot) \
        annot->_receiver(event); \
    } \
    DOUT("exit"); \
  }

  SEND_EVENT(sendContextMenuEvent, contextMenuEvent, QContextMenuEvent)
  SEND_EVENT(sendMouseMoveEvent, mouseMoveEvent, QMouseEvent)
  SEND_EVENT(sendMousePressEvent, mousePressEvent, QMouseEvent)
  SEND_EVENT(sendMouseReleaseEvent, mouseReleaseEvent, QMouseEvent)
  SEND_EVENT(sendMouseDoubleClickEvent, mouseDoubleClickEvent, QMouseEvent)

#undef SEND_EVENT

// - Message mode -

void
AnnotationGraphicsView::showAnnotationsAtPos(qint64 pos)
{
  DOUT("enter: pos =" << pos);
  emit removeItemRequested();

  emit annotationPosChanged(pos);

  if (!hub_->isSignalTokenMode()) {
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
    for (BOOST_AUTO(p, l.begin()); p != l.end(); ++p)
      showAnnotation(*p);

  DOUT("exit");
}

void
AnnotationGraphicsView::updateAnnotationText(const QString &text)
{
  qint64 id = editor()->id();
  if (id) {
    updateAnnotationTextWithId(text, id);
    emit annotationTextUpdatedWithId(text, id);
  }
}

void
AnnotationGraphicsView::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  AnnotationGraphicsItem *item = itemWithId(id);
  if (item) {
    item->annotation().setText(text);
    item->annotation().setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);
    item->invalidateAnnotation();
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
    BOOST_AUTO(a, dynamic_cast<AnnotationGraphicsItem *>(item));
    if (a && a->annotation().id() == id)
      return a;
  }
  return 0;
}

Annotation*
AnnotationGraphicsView::annotationWithId(qint64 id) const
{
  for (AnnotationHash::ConstIterator h = hash_.begin(); h != hash_.end(); ++h) {
    const AnnotationList &l = h.value();
    for (AnnotationList::ConstIterator p = l.begin(); p != l.end(); ++p)
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

WId
AnnotationGraphicsView::trackedWindow() const
{ return trackedWindow_; }

void
AnnotationGraphicsView::setTrackedWindow(WId hwnd)
{
  if (hwnd == videoView_->winId()
#ifdef WITH_WIN_HOOK
      ||  videoView_->containsWindow(hwnd)
#endif // WITH_WIN_HOOK
#ifdef Q_WS_WIN
      || QtWin::getChildWindows(videoView_->winId()).contains(hwnd)
#endif // Q_WS_WIN
      )
    hwnd = 0;

  if (trackedWindow_ != hwnd) {
     trackedWindow_ = hwnd;
     updateTrackingTimer();
     emit trackedWindowChanged(trackedWindow_);
  }
#ifdef WITH_WIN_MOUSEHOOK
  if (trackedWindow_)
    MouseHook::globalInstance()->start();
  else
    MouseHook::globalInstance()->stop();
#endif // WITH_WIN_MOUSEHOOK
}

void
AnnotationGraphicsView::updateTrackingTimer()
{
  if (isVisible() && !fullScreen_ && trackedWindow_)
    startTracking();
  else
    stopTracking();
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
  BOOST_AUTO(p, annots.begin());

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
  while (p != annots.end())
    l.append(*p++);

  foreach (Annotation a, annots)
    emit annotationAdded(a);
}

void
AnnotationGraphicsView::setScale(qreal value)
{
 // qreal r = value / scale_;
  //Base::scale(r, r);

  scale_ = value;
  emit scaleChanged(scale_);
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

// EOF
