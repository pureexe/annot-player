// acmainwindow.cc
// 2/17/2012

#include "project/common/acmainwindow.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#ifdef WITH_MODULE_ANIMATION
# include "module/animation/fadeanimation.h"
#endif // WITH_MODULE_ANIMATION
#ifdef WITH_MODULE_IMAGEFILTER
# include "module/imagefilter/rippleimagefilter.h"
# include <QtGui/QMouseEvent>
# include <QtGui/QPainter>
#endif // WITH_MODULE_IMAGEFILTER
#include <QtGui/QStatusBar>
#include <QtCore/QTimer>

#define DEBUG "acmainwindow"
#include "module/debug/debug.h"

#define SS_STATUSBAR_(_color, _weight) \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(_color) \
    SS_FONT_WEIGHT(_weight) \
  SS_END
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange,normal)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red,bold)
#define SS_STATUSBAR_NOTIFY     SS_STATUSBAR_(orange,bold)
#ifdef Q_WS_X11
# define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(black,normal)
#else
# define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(cyan,normal)
#endif //Q_WS_X11

enum { StatusMessageTimeout = 5000 };

// - Constructions -

AcMainWindow::AcMainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), autoHideMenuBar_(true), fadeAni_(0), fadeEnabled_(true),
    rippleEnabled_(false), rippleFilter_(0), rippleTimer_(0)
{
//#ifdef Q_WS_WIN
//  if (!AcUi::isAeroAvailable())
//#endif // Q_WS_WIN
//  { rippleEnabled_ = true; }

  AcUi::globalInstance()->setWindowStyle(this);
#ifdef WITH_MODULE_ANIMATION
  fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
#endif // WITH_MODULE_ANIMATION

  messageTimer_ = new QTimer(this);
  messageTimer_->setInterval(StatusMessageTimeout);
  messageTimer_->setSingleShot(true);
  connect(messageTimer_, SIGNAL(timeout()), statusBar(), SLOT(hide()));
  statusBar()->hide();

  connect(this, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(errorMessage(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);
}

// - Log -

void
AcMainWindow::showMessage(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_MESSAGE);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    if (autoHideMenuBar_)
      messageTimer_->start();
  }
}

void
AcMainWindow::error(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_ERROR);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    if (autoHideMenuBar_)
      messageTimer_->start();
  }
}

void
AcMainWindow::warn(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_WARNING);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    if (autoHideMenuBar_)
      messageTimer_->start();
  }
}

void
AcMainWindow::notify(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_NOTIFY);
  statusBar()->showMessage(text);
  if (!text.isEmpty()) {
    statusBar()->show();
    if (autoHideMenuBar_)
      messageTimer_->start();
  }
}

// - Actions -

void
AcMainWindow::setRippleEnabled(bool t)
{
#ifdef WITH_MODULE_IMAGEFILTER
#ifdef Q_WS_WIN
  if (AcUi::isAeroAvailable())
    return;
#endif // Q_WS_WIN
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
AcMainWindow::fadeOut()
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
AcMainWindow::mouseMoveEvent(QMouseEvent *event)
{
#ifdef WITH_MODULE_IMAGEFILTER
  if (rippleEnabled_ && rippleFilter_ && rippleFilter_->hasCenter())
    rippleFilter_->setCenter(mapFromGlobal(event->globalPos()));
#endif // WITH_MODULE_IMAGEFILTER
  Base::mouseMoveEvent(event);
}

void
AcMainWindow::mousePressEvent(QMouseEvent *event)
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
AcMainWindow::mouseReleaseEvent(QMouseEvent *event)
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
AcMainWindow::paintEvent(QPaintEvent *event)
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
AcMainWindow::setVisible(bool visible)
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
AcMainWindow::closeEvent(QCloseEvent *e)
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
