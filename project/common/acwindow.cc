// acwindow.cc
// 5/5/2012

#include "project/common/acwindow.h"
#include "project/common/acui.h"
#ifdef WITH_MODULE_ANIMATION
#  include "module/animation/fadeanimation.h"
#endif // WITH_MODULE_ANIMATION
#include <QtGui/QShortcut>
#include <QtCore/QTimer>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowCloseButtonHint )

AcWindow::AcWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f ? f : WINDOW_FLAGS), fadeAni_(0), fadeEnabled_(true)
{
  AcUi::globalInstance()->setWindowStyle(this);
#ifdef WITH_MODULE_ANIMATION
  fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
#endif // WITH_MODULE_ANIMATION

  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

// - Events -

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
  Base::setVisible(visible);
}
void
AcWindow::closeEvent(QCloseEvent *e)
{
#ifdef WITH_MODULE_ANIMATION
  if (fadeAni_->state() == QAbstractAnimation::Running)
    fadeAni_->stop();
#endif // WITH_MODULE_ANIMATION
  Base::closeEvent(e);
}

void
AcWindow::fadeOut()
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
