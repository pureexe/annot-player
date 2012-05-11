// magnifier.cc
// 5/11/2012

#include "module/magnifier/magnifier.h"
#ifdef WITH_MODULE_PIXMAPFILTER
#  include "module/pixmapfilter/pixmapripplefilter.h"
#endif // WITH_MODULE_PIXMAPFILTER
#ifdef WITH_MODULE_ANIMATION
#  include "module/animation/fadeanimation.h"
#endif // WITH_MODULE_ANIMATION
#include <QtGui>

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::FramelessWindowHint | \
  Qt::WindowStaysOnTopHint

#define MIN_SCALE       0.2
#define MAX_SCALE       10.0
#define DEFAULT_SCALE   2.0

// - Construction -

Magnifier::Magnifier(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), w_(0), scale_(DEFAULT_SCALE), rippleFilter_(0), fadeAni_(0)
{
  setWindowTitle(tr("Magnifier"));

  // See: http://stackoverflow.com/questions/1333610/displaying-translucent-irregular-shaped-windows-with-qt
  //setAttribute(Qt::WA_TranslucentBackground);

  resize(300, 300);

  repaintTimer_ = new QTimer(this);
  repaintTimer_->setInterval(1000 / 10); // 10 / secs
  connect(repaintTimer_, SIGNAL(timeout()), SLOT(repaint()));

  hideTimer_ = new QTimer(this);
  hideTimer_->setSingleShot(0);
  connect(hideTimer_, SIGNAL(timeout()), SLOT(hide()));

#ifdef WITH_MODULE_PIXMAPFILTER
  rippleFilter_ = new PixmapRippleFilter(this);
#endif // WITH_MODULE_PIXMAPFILTER

  QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect; {
    e->setBlurRadius(12);
    e->setOffset(1);
    e->setColor(Qt::black);
  } setGraphicsEffect(e);

  new QShortcut(QKeySequence("ESC"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

// - Events -

void
Magnifier::fadeOut()
{
#ifdef WITH_MODULE_ANIMATION
  if (!fadeAni_)
    fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
  fadeAni_->fadeOut();
  hideTimer_->start(fadeAni_->duration());
#else
  hide();
#endif // WITH_MODULE_ANIMATION
}

void
Magnifier::setVisible(bool visible)
{
  if (visible) {
#ifdef WITH_MODULE_ANIMATION
    if (!fadeAni_)
      fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
    fadeAni_->fadeIn();
#endif // WITH_MODULE_ANIMATION
    hideTimer_->stop();
    repaintTimer_->start();
  } else {
    repaintTimer_->stop();
#ifdef WITH_MODULE_ANIMATION
    if (fadeAni_ && fadeAni_->state() != QAbstractAnimation::Stopped)
      fadeAni_->stop();
#endif // WITH_MODULE_ANIMATION
#ifdef WITH_MODULE_PIXMAPFILTER
    rippleFilter_->clearCenter();
#endif // WITH_MODULE_PIXMAPFILTER
  }
  Base::setVisible(visible);
  if (visible)
    repaint();
}

void
Magnifier::paintEvent(QPaintEvent *e)
{
  if (!w_) {
    Base::paintEvent(e);
    return;
  }

  QRect r(0, 0, qMax(width()/scale_,1.0), qMax(height()/scale_,1.0));
  r.moveCenter(center_);
  QPixmap pm = QPixmap::grabWindow(w_->winId(), r.x(), r.y(), r.width(), r.height());
  if (pm.isNull())
    return;
  pm = pm.scaled(size());

  QPainter painter(this);
#ifdef WITH_MODULE_PIXMAPFILTER
  if (rippleFilter_->needsDisplay())
    rippleFilter_->draw(&painter, QPoint(), pm);
  else
#endif // WITH_MODULE_PIXMAPFILTER
  painter.drawPixmap(0, 0, pm);
}

void
Magnifier::contextMenuEvent(QContextMenuEvent *event)
{
  Q_UNUSED(event);
  fadeOut();
}

void
Magnifier::mouseDoubleClickEvent(QMouseEvent *event)
{ Base::mouseDoubleClickEvent(event); }

void
Magnifier::mousePressEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::MiddleButton: scale_ = DEFAULT_SCALE; break;
  case Qt::RightButton: fadeOut(); break;
  default: ;
  }
  event->accept();
  Base::mousePressEvent(event);
#ifdef WITH_MODULE_PIXMAPFILTER
  rippleFilter_->setDampling(16);
  rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_PIXMAPFILTER
}

void
Magnifier::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
  Base::mouseMoveEvent(event);
#ifdef WITH_MODULE_PIXMAPFILTER
  if (rippleFilter_->hasCenter())
    rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_PIXMAPFILTER
}

void
Magnifier::mouseReleaseEvent(QMouseEvent *event)
{
  event->accept();
  Base::mouseReleaseEvent(event);
#ifdef WITH_MODULE_PIXMAPFILTER
  rippleFilter_->setDampling(4);
  rippleFilter_->clearCenter();
#endif // WITH_MODULE_PIXMAPFILTER
}

void
Magnifier::wheelEvent(QWheelEvent *event)
{
  Base::wheelEvent(event);
#ifdef WITH_MODULE_PIXMAPFILTER
  if (event->delta() > 0) {
    if (scale_ < MAX_SCALE)
      scale_ *= 1.1;
  } else if (event->delta() < 0) {
    if (scale_ > MIN_SCALE)
      scale_ /= 1.1;
  }
#endif // WITH_MODULE_PIXMAPFILTER
}

// EOF
