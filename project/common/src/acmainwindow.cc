// acmainwindow.cc
// 2/17/2012

#include "ac/acmainwindow.h"
#include "ac/acss.h"
#include "ac/acui.h"
#include <QStatusBar>
#include <QTimer>

#define SS_STATUSBAR_(_color) \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(_color) \
  SS_END
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red)
#define SS_STATUSBAR_NOTIFY     SS_STATUSBAR_(red)
#ifdef Q_WS_X11
#  define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(black)
#else
#  define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(cyan)
#endif //Q_WS_X11

enum { StatusMessageTimeout = 5000 };

// - Constructions -


AcMainWindow::AcMainWindow(QWidget *parent, Qt::WindowFlags f)
  : Base(parent, f)
{
  AcUi::globalInstance()->setWindowStyle(this);

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
  statusBar()->show();
  messageTimer_->start();
}

void
AcMainWindow::error(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_ERROR);
  statusBar()->showMessage(text);
  statusBar()->show();
  messageTimer_->start();
}

void
AcMainWindow::warn(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_WARNING);
  statusBar()->showMessage(text);
  statusBar()->show();
  messageTimer_->start();
}

void
AcMainWindow::notify(const QString &text)
{
  statusBar()->setStyleSheet(SS_STATUSBAR_NOTIFY);
  statusBar()->showMessage(text);
  statusBar()->show();
  messageTimer_->start();
}

// EOF
