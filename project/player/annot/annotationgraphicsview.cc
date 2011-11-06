// annotationgraphicsview.cc
// 7/14/2011

#include "annotationgraphicsview.h"
#include "annotationgraphicsitem.h"
#include "annotationeditor.h"
#include "stylesheet.h"
#include "signalhub.h"
#include "videoview.h"
#include "logger.h"
#include "global.h"
#include "module/player/player.h"
#ifdef USE_WIN_QTWIN
  #include "win/qtwin/qtwin.h"
#endif // USE_WIN_QTWIN
#include <QtGui>
#include <boost/typeof/typeof.hpp>
#include <climits>

#define DEBUG "AnnotationGraphicsView"
#include "module/debug/debug.h"

using namespace Core::Cloud;
using namespace Logger;

// - Constructions -

AnnotationGraphicsView::AnnotationGraphicsView(SignalHub *hub, Player *player, VideoView *view, QWidget *parent)
  : Base(parent), videoView_(view), fullScreenView_(0), trackingWindow_(0),
    hub_(hub), player_(player), active_(false), paused_(false),
    playTime_(-1), languages_(0), userId_(0), playbackEnabled_(true), subtitlePosition_(AP_Bottom)
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

  editor_ = new AnnotationEditor(this);
  connect(editor_, SIGNAL(textSaved(QString)), SLOT(updateAnnotationText(QString)));

  trackingTimer_ = new QTimer(this);
  trackingTimer_->setInterval(G_TRACKING_INTERVAL);
  connect(trackingTimer_, SIGNAL(timeout()), SLOT(invalidateGeometry()));

  //centerOn(0, 0);
}

AnnotationGraphicsView::AnnotationPosition
AnnotationGraphicsView::subtitlePosition() const
{ return subtitlePosition_; }

void
AnnotationGraphicsView::setSubtitlePosition(AnnotationPosition ap)
{ subtitlePosition_ = ap; }

qint64
AnnotationGraphicsView::languages() const
{ return languages_; }

void
AnnotationGraphicsView::setLanguages(qint64 bits)
{ languages_ = bits; }

qint64
AnnotationGraphicsView::userId() const
{ return userId_; }

void
AnnotationGraphicsView::setUserId(qint64 uid)
{ userId_ = uid; }

AnnotationEditor*
AnnotationGraphicsView::editor() const
{ return editor_; }

AnnotationGraphicsView::~AnnotationGraphicsView()
{ clearAnnotations(); }

void
AnnotationGraphicsView::setFullScreenView(QWidget *w)
{ fullScreenView_ = w; }

void
AnnotationGraphicsView::clearAnnotations()
{
  if (!annots_.empty()) {
    QGraphicsScene *s = scene();
    Q_ASSERT(s);
    foreach (QGraphicsItem *item, s->items())
      s->removeItem(item);

    foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
      if (l) {
        if (!l->empty())
          foreach (AnnotationGraphicsItem *item, *l)
            if (item)
              delete item;
        delete l;
      }

    annots_.clear();
  }
}

void
AnnotationGraphicsView::setAnnotations(const AnnotationList &annots)
{
  clearAnnotations();
  addAnnotations(annots);
}

AnnotationList
AnnotationGraphicsView::annotations() const
{
  AnnotationList ret;
  if (!annots_.empty())
    foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
      if (l && !l->empty())
        foreach (AnnotationGraphicsItem *item, *l)
          if (item)
            ret.append(item->annotation());
  return ret;
}

AnnotationList
AnnotationGraphicsView::annotationsAtPos(qint64 pos) const
{
  AnnotationList ret;
  if (annots_.contains(pos)) {
    QList<AnnotationGraphicsItem*> *l = annots_[pos];
    if (l && !l->empty())
      foreach (AnnotationGraphicsItem *item, *l)
        ret.append(item->annotation());
  }
  return ret;
}

// - View tracking -

void
AnnotationGraphicsView::invalidateGeometry()
{
  invalidateSize();
  invalidatePos();
}

void
AnnotationGraphicsView::invalidateSize()
{
  if (trackingWindow_) {
#ifdef USE_WIN_QTWIN
    QRect r = QtWin::getWindowRect(trackingWindow_);
    if (r.isNull()) {
      if (!QtWin::isValidWindow(trackingWindow_))
        setTrackingWindow(0);
    } else if (size() != r.size())
      resize(r.size());
#endif // USE_WIN_QTWIN
  } else if (hub_->isMediaTokenMode() || hub_->isNormalPlayerMode()) {
    // FIXME: 10/24/2011: Screen not working orz
#ifdef Q_WS_MAC
    if (fullScreenView_)
      resize(fullScreenView_->size());
#else
    resize(videoView_->size());
#endif // Q_WS_MAC

  } else if (hub_->isSignalTokenMode()) {
    if (fullScreenView_)
      resize(fullScreenView_->size());
  }

  setSceneRect(0, 0, width(), height());
}

void
AnnotationGraphicsView::invalidatePos()
{
  if (trackingWindow_) {
#ifdef USE_WIN_QTWIN
    QRect r = QtWin::getWindowRect(trackingWindow_);
    if (r.isNull()) {
      if (!QtWin::isValidWindow(trackingWindow_))
        setTrackingWindow(0);
    } else {
      QPoint newPos = r.topLeft();
      moveToGlobalPos(newPos);
      return;
    }
#endif // USE_WIN_QTWIN
  } else if (hub_->isMediaTokenMode() || hub_->isNormalPlayerMode()) {
#ifdef Q_WS_MAC
    if (fullScreenView_)
      move(fullScreenView_->pos());
    // FIXME: 10/24/2011: Screen not working orz. Failed to compute exact location
    /*
    QPoint g_to = videoView_->mapToGlobal(QPoint());
    QPoint g_from = mapToGlobal(pos());

    move(frameGeometry().topLeft() + pos()  // relative position
         - QPoint(0, videoView_->height())       // Fix height taken by NSView in QMacCocoaView
         + g_to - g_from);                  // absolute distance
         */

#else
    QPoint newPos = videoView_->mapToGlobal(QPoint());
    moveToGlobalPos(newPos);

#endif // Q_WS_MAC
  } else if (hub_->isSignalTokenMode()) {
    move(fullScreenView_->pos());
  }
}

void
AnnotationGraphicsView::moveToGlobalPos(const QPoint &globalPos)
{
  // Currently only work on Windows
  QPoint newPos = frameGeometry().topLeft() + pos() // relative position
                  + globalPos - mapToGlobal(pos()); // absolute distance
  if (newPos != pos())
    move(newPos);
}

void
AnnotationGraphicsView::setVisible(bool visible)
{
  setActive(visible);
  if (visible)
    invalidateGeometry();
  Base::setVisible(visible);
}

// - Player connections -

#define SET_PLAYER_CONNECTIONS(_connect) \
  void \
  AnnotationGraphicsView::_connect##Player() \
  { \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(invalidateAnnotations())); \
    _connect(player_, SIGNAL(timeChanged()), this, SLOT(invalidatePlayTime())); \
    _connect(player_, SIGNAL(paused()), this, SLOT(pause())); \
    _connect(player_, SIGNAL(playing()), this, SLOT(resume())); \
    _connect(player_, SIGNAL(stopped()), this, SLOT(resume())); \
    _connect(player_, SIGNAL(stopped()), this, SLOT(invalidateAnnotationPos())); \
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
    _connect(hub_, SIGNAL(stopped()), this, SLOT(invalidateAnnotationPos())); \
  }

  SET_HUB_CONNECTIONS(connect)
  SET_HUB_CONNECTIONS(disconnect)
#undef HUB_CONNECTIONS

bool
AnnotationGraphicsView::isActive() const
{ return active_; }

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

bool
AnnotationGraphicsView::isPlaybackEnabled() const
{ return playbackEnabled_; }

void
AnnotationGraphicsView::setPlaybackEnabled(bool enabled)
{
  playbackEnabled_ = enabled;
  emit playbackEnabledChanged(enabled);
}

void
AnnotationGraphicsView::togglePlaybackEnabled()
{ setPlaybackEnabled(!playbackEnabled_); }

// - Pause/resme -

bool
AnnotationGraphicsView::isPaused() const
{ return paused_; }

void
AnnotationGraphicsView::pause()
{
  if (!paused_) {
    paused_ = true;
    emit paused();
  }
}

void
AnnotationGraphicsView::resume()
{
  if (paused_) {
    paused_ = false;
    emit resumed();
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
AnnotationGraphicsView::invalidatePlayTime()
{
  if (!player_->hasMedia()
      || !playbackEnabled_)
    return;

  qint64 msecs = player_->time();
  if (msecs < 0)
    return;

  // jichi 7/27/2011: Expect this to be a problem
  qint64 secs = msecs / 1000;
  if (secs != playTime_) { // playTime_ is used to prevent the same seconds being double displayed
    playTime_ = secs;
    showAnnotationsAtPos(secs * 1000);
  }
}

void
AnnotationGraphicsView::invalidateAnnotations()
{
  clearAnnotations();
  /*
  if (player_->hasMedia()) {
    qint64 msecs = player_->mediaLength();
    if (msecs < 0)
      return;
    int secs = ::msecs2secs(msecs);
    annots_.resize(secs + 1); // +1 to make sure the last seconds can be held
  }
  */

  playTime_ = -1;
}

void
AnnotationGraphicsView::addAnnotations(const AnnotationList &annots)
{
  DOUT("addAnnotations:enter: size =" << annots.size());
  if (!annots.empty())
    foreach (const Annotation &a, annots)
      addAnnotation(a);
  DOUT("addAnnotations:exit");
}

void
AnnotationGraphicsView::addAnnotation(const Annotation &annot, qint64 delaysecs)
{
  DOUT("addAnnotation:enter: aid =" << annot.id() << ", pos =" << annot.pos());
  AnnotationGraphicsItem *item = new AnnotationGraphicsItem(annot, hub_, this);

  qint64 pos = annot.pos();
  if (hub_->isMediaTokenMode())
    pos = pos / 1000; // msecs => secs, cluster media annotation by seconds

  QList<AnnotationGraphicsItem*> *l = annots_[pos];
  if (!l)
    l = annots_[pos] = new QList<AnnotationGraphicsItem*>;
  l->append(item);

  if (isItemBlocked(item))
      return;

  if (delaysecs == LLONG_MAX)
    item->showMe();
  else if (hub_->isMediaTokenMode() && player_->hasMedia()) {
    qint64 secsdiff =  player_->time() / 1000 - annot.pos() / 1000;
    if (secsdiff >= 0 && secsdiff <= delaysecs)
      item->showMe();
  }
}

void
AnnotationGraphicsView::addAndShowAnnotation(const Annotation &annot)
{ addAnnotation(annot, LLONG_MAX); }

void
AnnotationGraphicsView::showAnnotation(const Annotation &annot)
{
  AnnotationGraphicsItem *item = new AnnotationGraphicsItem(annot, hub_, this);
  if (!isItemBlocked(item))
    item->showMe();
}

// - Queries -

int
AnnotationGraphicsView::itemsCount() const
{
  int ret = 0;
  foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
    if (l)
      ret += l->size();

  return ret;
}

int
AnnotationGraphicsView::itemsCount(int time) const
{
  int ret = 0;
  BOOST_AUTO(it, annots_.find(time));
  if (it != annots_.end()) {
    BOOST_AUTO(l, *it);
    if (l)
      ret = l->size();
  }
  return ret;
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
    DOUT(# _event ": enter"); \
    if (event) { \
      QGraphicsItem *item = itemAt(mapFromGlobal(event->globalPos())); \
      AnnotationGraphicsItem *annot = dynamic_cast<AnnotationGraphicsItem*>(item); \
      if (annot) \
        annot->_receiver(event); \
    } \
    DOUT(# _event ": exit"); \
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
  DOUT("showAnnotationsAtPos:enter: pos =" << pos);
  if (hub_->isMediaTokenMode())
    pos = pos / 1000; // msecs => secs

  //qDebug() << annots_.keys();

  if (annots_.contains(pos)) {
    DOUT("showAnnotationsAtPos: found annotations at pos =" << pos);
    QList<AnnotationGraphicsItem*> *l = annots_[pos];
    if (l && !l->empty())
      foreach (AnnotationGraphicsItem *item, *l)
       if (item && !isItemBlocked(item))
         item->showMe();
  }

  emit posChanged();

  DOUT("showAnnotationsAtPos:exit");
}

void
AnnotationGraphicsView::invalidateAnnotationPos()
{ emit posChanged(); }

void
AnnotationGraphicsView::updateAnnotationText(const QString &text)
{
  qint64 id = editor_->id();
  if (id) {
    if (id != userId_) {
      warn(tr("cannot edit other's annotation text"));
      return;
    }

    AnnotationGraphicsItem *item = itemWithId(id);
    if (item) {
      Annotation a = item->annotation();
      a.setText(text);
      item->setAnnotation(a);
    }
    emit annotationTextUpdated(text, id);
  }
}

AnnotationGraphicsItem*
AnnotationGraphicsView::itemWithId(qint64 aid) const
{
  if (!annots_.empty())
    foreach (QList<AnnotationGraphicsItem*> *l, annots_.values())
      if (l && !l->empty())
        foreach (AnnotationGraphicsItem *item, *l)
          if (item && item->annotation().id() == aid)
            return item;
  return 0;
}

bool
AnnotationGraphicsView::isItemBlocked(const AnnotationGraphicsItem *item) const
{
  if (!item)
    return true;

  if ((languages_ | Traits::AnyLanguageBit) == 0) {
    qint32 l = item->annotation().language();
    if (((2 << l) & languages_) == 0)
      return true;
  }

  return false;
}

// - Tracking -

WId
AnnotationGraphicsView::trackingWindow() const
{ return trackingWindow_; }

void
AnnotationGraphicsView::setTrackingWindow(WId hwnd)
{
  if (hwnd == videoView_->winId()
#ifdef USE_WIN_HOOK
      ||  videoView_->containsWindow(hwnd)
#endif // USE_WIN_HOOK
      )
    hwnd = 0;

  if (trackingWindow_ != hwnd) {
     trackingWindow_ = hwnd;
     if (trackingWindow_)
       startTracking();
     else
       stopTracking();
     emit trackingWindowChanged(trackingWindow_);
  }
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

// EOF
