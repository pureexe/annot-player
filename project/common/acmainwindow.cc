// acmainwindow.cc
// 2/17/2012

#include "project/common/acmainwindow.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#ifdef WITH_MODULE_ANIMATION
#  include "module/animation/fadeanimation.h"
#endif // WITH_MODULE_ANIMATION
#include <QtGui/QStatusBar>
#include <QtCore/QTimer>

#define SS_STATUSBAR_(_color, _weight) \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(_color) \
    SS_FONT_WEIGHT(_weight) \
  SS_END
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange,normal)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red,bold)
#define SS_STATUSBAR_NOTIFY     SS_STATUSBAR_(orange,bold)
#ifdef Q_WS_X11
#  define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(black,normal)
#else
#  define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(cyan,normal)
#endif //Q_WS_X11

enum { StatusMessageTimeout = 5000 };

// - Constructions -

AcMainWindow::AcMainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f), autoHideMenuBar_(true), fadeAni_(0), fadeEnabled_(true)
{
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

// - Events -

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
  Base::setVisible(visible);
}

void
AcMainWindow::closeEvent(QCloseEvent *e)
{
#ifdef WITH_MODULE_ANIMATION
  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();
#endif // WITH_MODULE_ANIMATION
  Base::closeEvent(e);
}

void
AcMainWindow::fadeOut()
{
#ifdef WITH_MODULE_ANIMATION
  if (fadeEnabled_ && isVisible()) {
    fadeAni_->fadeOut(windowOpacity());
    QTimer::singleShot(fadeAni_->duration(), this, SLOT(hide()));
  } else
#endif // WITH_MODULE_ANIMATION
  hide();
}

// EOF
