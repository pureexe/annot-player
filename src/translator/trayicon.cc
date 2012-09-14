// systemtrayicon.cc
// 4/1/2012

#include "trayicon.h"
#include "mainwindow.h"
#include "rc.h"
#include <QtGui>

// - Construction -

TrayIcon::TrayIcon(MainWindow *w, QObject *parent)
  : Base(QIcon(RC_IMAGE_APP), parent), w_(w)
{
  Q_ASSERT(w_);
  setToolTip(tr("Annot Translator"));

  createActions();
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(activate(QSystemTrayIcon::ActivationReason)));
}

void
TrayIcon::createActions()
{
  QMenu *m = new QMenu(w_);
  showAct_ = m->addAction(tr("Show"), w_, SLOT(show()));
  hideAct_ = m->addAction(tr("Hide"), w_, SLOT(fadeOut()));
  m->addAction(tr("Quit"), w_, SLOT(quit()));
  setContextMenu(m);
}

// - Events -

void
TrayIcon::activate(ActivationReason reason)
{
  switch (reason) {
  case Context:
    updateContextMenu();
    break;
  case Trigger:
  case DoubleClick:
  case MiddleClick:
    w_->show();
    break;
    break;
  default: ;
  }
}

void
TrayIcon::updateContextMenu()
{
  bool v = w_->isVisible();
  showAct_->setVisible(!v);
  hideAct_->setVisible(v);
}

// EOF

/*
void
TrayIcon::setWindowStaysOnTop(bool t)
{
  if (t != w_->isWindowStaysOnTop()) {
    const char *slot = t ? SLOT(setWindowStaysOnTopToTrue())
                         : SLOT(setWindowStaysOnTopToFalse());
    QTimer::singleShot(0, this, slot);
  }
}

void
TrayIcon::setWindowStaysOnTopToTrue()
{ w_->setWindowStaysOnTop(true); }

void
TrayIcon::setWindowStaysOnTopToFalse()
{ w_->setWindowStaysOnTop(false); }

*/
