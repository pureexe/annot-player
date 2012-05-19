// magnifier.cc
// 5/11/2012

#include "module/magnifier/magnifier.h"
#ifdef WITH_MODULE_IMAGEFILTER
#  include "module/imagefilter/darknessimagefilter.h"
#  include "module/imagefilter/fogimagefilter.h"
#  include "module/imagefilter/rippleimagefilter.h"
#endif // WITH_MODULE_IMAGEFILTER
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

#define MAX_SIZE        QSize(800,800)
#define MIN_SIZE        QSize(200,200)
#define DEFAULT_SIZE    QSize(400,400)

// - Construction -

Magnifier::Magnifier(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), w_(0), scale_(DEFAULT_SCALE),
    darknessFilter_(0), fogFilter_(0), mosaicFilter_(0), rippleFilter_(0),
    fadeAni_(0),
    contextMenu_(0), darknessFilterAct_(0), fogFilterAct_(0), mosaicFilterAct_(0)
{
  setWindowTitle(tr("Magnifier"));

  // See: http://stackoverflow.com/questions/1333610/displaying-translucent-irregular-shaped-windows-with-qt
  //setAttribute(Qt::WA_TranslucentBackground);

  resize(DEFAULT_SIZE);

  repaintTimer_ = new QTimer(this);
  repaintTimer_->setInterval(1000 / 10); // 10fps
  connect(repaintTimer_, SIGNAL(timeout()), SLOT(repaint()));

  hideTimer_ = new QTimer(this);
  hideTimer_->setSingleShot(0);
  connect(hideTimer_, SIGNAL(timeout()), SLOT(hide()));

#ifdef WITH_MODULE_IMAGEFILTER
  darknessFilter_ = new DarknessImageFilter(this);
  fogFilter_ = new FogImageFilter(this);
  rippleFilter_ = new RippleImageFilter(this);
#endif // WITH_MODULE_IMAGEFILTER

  QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect; {
    e->setBlurRadius(12);
    e->setOffset(1);
    e->setColor(Qt::black);
  } setGraphicsEffect(e);

#ifdef WITH_MODULE_IMAGEFILTER
  darknessFilterAct_ = new QAction(tr("Reduce Darkness"), this);
  darknessFilterAct_->setCheckable(true);

  fogFilterAct_ = new QAction(tr("Reduce Fog"), this);
  fogFilterAct_->setCheckable(true);
  //fogFilterAct_->setChecked(true);

  mosaicFilterAct_ = new QAction(tr("Remove Mosaic"), this);
  mosaicFilterAct_->setCheckable(true);
#endif // WITH_MODULE_IMAGEFILTER

  new QShortcut(QKeySequence("ESC"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

// - Events -

void
Magnifier::fadeOut()
{
  if (isVisible()) {
#ifdef WITH_MODULE_ANIMATION
    if (!fadeAni_)
      fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
    fadeAni_->fadeOut();
    hideTimer_->start(fadeAni_->duration());
#else
    hide();
#endif // WITH_MODULE_ANIMATION
  }
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
#ifdef WITH_MODULE_IMAGEFILTER
    rippleFilter_->clearCenter();
#endif // WITH_MODULE_IMAGEFILTER
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

  QRect desktop = QApplication::desktop()->geometry();
  QRect r(0, 0, qMax(width()/scale_,1.0), qMax(height()/scale_,1.0));
  r.moveCenter(center_);
  if (!desktop.contains(r))
    r = desktop.intersected(r);
  QPixmap pm = QPixmap::grabWindow(w_->winId(), r.x(), r.y(), r.width(), r.height());
  if (pm.isNull())
    return;
  pm = pm.scaled(size());

  QPainter painter(this);
#ifdef WITH_MODULE_IMAGEFILTER
  bool darknessNeeded = darknessFilterAct_->isChecked() && darknessFilter_->needsDisplay(),
       fogNeeded = fogFilterAct_->isChecked() && fogFilter_->needsDisplay(),
       rippleNeeded = rippleFilter_->needsDisplay();
  if (!(darknessNeeded || fogNeeded || rippleNeeded))
    painter.drawPixmap(0, 0, pm);
  else {
    QImage image = pm.toImage();
#if 0 // CHECKPOINT
    if (darknessNeeded)
      darknessFilter_->filterImage(image);
    if (fogNeeded)
      fogFilter_->filterImage(image);
#endif // 0
    if (rippleNeeded)
      rippleFilter_->drawImage(painter, image);
    else
      painter.drawImage(0, 0, image);
  }
#else
  painter.drawPixmap(0, 0, pm);
#endif // WITH_MODULE_IMAGEFILTER
}

void
Magnifier::contextMenuEvent(QContextMenuEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (!contextMenu_) {
    contextMenu_ = new QMenu(this);
    contextMenu_->addAction(fogFilterAct_);
    contextMenu_->addAction(darknessFilterAct_);
    contextMenu_->addAction(mosaicFilterAct_);
    contextMenu_->addSeparator();
    contextMenu_->addAction(tr("Close") + " [" + tr("DoubleClick") + "]",
                            this, SLOT(fadeOut()));
  }
  contextMenu_->exec(event->globalPos());
  event->accept();
#else
  Q_UNUSED(event);
  fadeOut();
#endif WITH_MODULE_IMAGEFILTER
}

void
Magnifier::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton && !event->modifiers())
    fadeOut();
  event->accept();
}

void
Magnifier::mousePressEvent(QMouseEvent *event)
{
  switch (event->button()) {
  case Qt::MiddleButton:
    if (event->modifiers()) {
      QSize sz = DEFAULT_SIZE;
      if (sz != size()) {
        move(pos() + QPoint(width() - sz.width(), height() - sz.height()));
        resize(sz);
      }
    } else
      scale_ = DEFAULT_SCALE;
    break;
  default: ;
  }
#ifdef WITH_MODULE_IMAGEFILTER
  rippleFilter_->setDampling(16);
  rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_IMAGEFILTER
  event->accept();
  Base::mousePressEvent(event);
}

void
Magnifier::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleFilter_->hasCenter())
    rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_IMAGEFILTER
  Base::mouseMoveEvent(event);
}

void
Magnifier::mouseReleaseEvent(QMouseEvent *event)
{
  event->accept();
#ifdef WITH_MODULE_IMAGEFILTER
  rippleFilter_->setDampling(4);
  rippleFilter_->clearCenter();
#endif // WITH_MODULE_IMAGEFILTER
  Base::mouseReleaseEvent(event);
}

void
Magnifier::wheelEvent(QWheelEvent *event)
{
  if (event->delta()) {
    if (event->modifiers()) {
      QSize sz = size();
      if (event->delta() > 0) {
        if (sz.width() < MAX_SIZE.width() && sz.height() < MAX_SIZE.height())
          sz*= 1.1;
      } else {
        if (sz.width() > MIN_SIZE.width() && sz.height() > MIN_SIZE.height())
          sz /= 1.1;
      }
      if (sz != size()) {
        move(pos() + QPoint(width() - sz.width(), height() - sz.height()));
        resize(sz);
      }
    } else {
      if (event->delta() > 0) {
        if (scale_ < MAX_SCALE)
          scale_ *= 1.1;
      } else {
        if (scale_ > MIN_SCALE)
          scale_ /= 1.1;
      }
    }
  }
  Base::wheelEvent(event);
}

// - Actions -

void
Magnifier::trigger()
{
#ifdef WITH_MODULE_IMAGEFILTER
  rippleFilter_->setCenter(width()/2, height()/2);
  repaint();
  QTimer::singleShot(0, rippleFilter_, SLOT(clearCenter()));;
#endif // WITH_MODULE_IMAGEFILTER
}

// EOF
