// acwindow.cc
// 5/5/2012

#include "project/common/acwindow.h"
#include "project/common/acui.h"
#ifdef WITH_MODULE_ANIMATION
# include "module/animation/fadeanimation.h"
#endif // WITH_MODULE_ANIMATION
#ifdef WITH_MODULE_IMAGEFILTER
# include "module/imagefilter/rippleimagefilter.h"
# include <QtGui/QMouseEvent>
# include <QtGui/QPainter>
#endif // WITH_MODULE_IMAGEFILTER
#include <QtGui/QShortcut>
#include <QtCore/QTimer>

// - Constructions -

AcWindow::AcWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f ? f : AC_WINDOW_FLAGS), fadeAni_(0), fadeEnabled_(true),
    rippleEnabled_(false), rippleFilter_(0), rippleTimer_(0)
{
//#ifdef Q_OS_WIN
//  if (!AcUi::isAeroAvailable())
//#endif // Q_OS_WIN
//  { rippleEnabled_ = true; }

  AcUi::globalInstance()->setWindowStyle(this);
#ifdef WITH_MODULE_ANIMATION
  fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
#endif // WITH_MODULE_ANIMATION

  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

// - Properties -

void
AcWindow::setWindowOnTop(bool t)
{
  if (t != isWindowOnTop()) {
    bool visible = isVisible();
    setWindowFlags(t ? windowFlags() | Qt::WindowStaysOnTopHint :
                       windowFlags() & ~Qt::WindowStaysOnTopHint);

#ifdef WITH_WIN_DWM
    AcUi::globalInstance()->setDwmEnabled(true);
#endif // WITH_WIN_DWM
    if (visible)
      show();
  }
}

void
AcWindow::setRippleEnabled(bool t)
{
#ifdef WITH_MODULE_IMAGEFILTER
#ifdef Q_OS_WIN
  if (AcUi::isAeroAvailable())
    return;
#endif // Q_OS_WIN
  if (t == rippleEnabled_)
    return;
  if (t)
    AcUi::globalInstance()->removeWindowBackground(this);
  else
    AcUi::globalInstance()->setWindowBackground(this, false); // persistent is false
  rippleEnabled_ = t;
  if (t) {
    if (rippleTimer_ && !rippleTimer_->isActive())
      rippleTimer_->start();
  } else {
    if (rippleTimer_)
      rippleTimer_->stop();
    if (rippleFilter_)
      rippleFilter_->clear();
  }
#else
  rippleEnabled_ = t;
#endif // WITH_MODULE_IMAGEFILTER
}

void
AcWindow::fadeOut()
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleTimer_ && rippleTimer_->isActive())
    rippleTimer_->stop();
#endif // WITH_MODULE_IMAGEFILTER
#ifdef WITH_MODULE_ANIMATION
  if (fadeEnabled_ && isVisible()) {
    fadeAni_->fadeOut(windowOpacity());
    QTimer::singleShot(fadeAni_->duration(), this, SLOT(hide()));
  } else
#endif // WITH_MODULE_ANIMATION
  hide();
}

// - Events -

void
AcWindow::mouseMoveEvent(QMouseEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleEnabled_ && rippleFilter_ && rippleFilter_->hasCenter())
    rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_IMAGEFILTER
  Base::mouseMoveEvent(event);
}

void
AcWindow::mousePressEvent(QMouseEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleEnabled_ && rippleFilter_ && event->button() != Qt::RightButton) {
    rippleFilter_->setDampling(16);
    rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
  }
#endif // WITH_MODULE_IMAGEFILTER
  Base::mousePressEvent(event);
}

void
AcWindow::mouseReleaseEvent(QMouseEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleEnabled_ && rippleFilter_) {
     rippleFilter_->clearCenter();
     rippleFilter_->setDampling(4);
  }
#endif // WITH_MODULE_IMAGEFILTER
  Base::mouseReleaseEvent(event);
}

void
AcWindow::paintEvent(QPaintEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (!rippleEnabled_) {
    Base::paintEvent(event);
    return;
  }
  //DOUT("enter");
  if (!rippleTimer_) {
    rippleTimer_ = new QTimer(this);
    rippleTimer_->setInterval(1000 / 30); // 30fps
    connect(rippleTimer_, SIGNAL(timeout()), SLOT(repaint()));
    rippleTimer_->start();
  }
  if (!rippleFilter_)
    rippleFilter_ = new RippleImageFilter(this);

  static const QImage image(AcUi::globalInstance()->backgroundImage());
  QPainter painter(this);
  if (rippleFilter_->needsDisplay())
    rippleFilter_->drawImage(painter, image);
  else
    painter.drawImage(0, 0, image);
  //DOUT("exit");
#else
  Base::paintEvent(event);
#endif // WITH_MODULE_IMAGEFILTER
}

void
AcWindow::setVisible(bool visible)
{
#ifdef WITH_MODULE_ANIMATION
  if (fadeEnabled_ && visible && !isVisible()) {
    static qreal opacity = 0.0;
    if (qFuzzyCompare(opacity + 1, 1))
      opacity = windowOpacity();
    fadeAni_->fadeIn(opacity);
  }
#endif // WITH_MODULE_ANIMATION
#ifdef WITH_MODULE_IMAGEFILTER
  if (visible) {
    if (rippleTimer_ && !rippleTimer_->isActive())
      rippleTimer_->start();
  } else {
    if (rippleTimer_ && rippleTimer_->isActive())
      rippleTimer_->stop();
    if (rippleFilter_)
      rippleFilter_->clear();
  }
#endif // WITH_MODULE_IMAGEFILTER
  Base::setVisible(visible);
}

void
AcWindow::closeEvent(QCloseEvent *e)
{
#ifdef WITH_MODULE_ANIMATION
  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();
#endif // WITH_MODULE_ANIMATION
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleTimer_ && rippleTimer_->isActive())
    rippleTimer_->stop();
#endif // WITH_MODULE_IMAGEFILTER
  Base::closeEvent(e);
}

// EOF
