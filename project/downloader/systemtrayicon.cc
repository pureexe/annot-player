// systemtrayicon.cc
// 4/1/2012

#include "systemtrayicon.h"
#include "mainwindow.h"
#include "rc.h"
#include <QtGui>

// - Construction -

SystemTrayIcon::SystemTrayIcon(MainWindow *w, QObject *parent)
  : Base(parent), w_(w)
{
  Q_ASSERT(w_);
  setIcon(QIcon(RC_IMAGE_APP));
  setToolTip(tr("Annot Downloader"));

  createActions();

  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(activate(QSystemTrayIcon::ActivationReason)));
}

void
SystemTrayIcon::createActions()
{
  // Actions
  QAction *addAct = new QAction(tr("Add"), this);
  connect(addAct, SIGNAL(triggered()), w_, SLOT(add()));

  QAction *showAct = new QAction(tr("Show"), this);
  connect(showAct, SIGNAL(triggered()), w_, SLOT(show()));

  QAction *quitAct = new QAction(tr("Quit"), this);
  connect(quitAct, SIGNAL(triggered()), w_, SLOT(close()));

  // Menu
  QMenu *menu = new QMenu(w_); {
    menu->addAction(addAct);
    menu->addSeparator();
    menu->addAction(showAct);
    menu->addAction(quitAct);
  }
  setContextMenu(menu);
}

// - Events -

void
SystemTrayIcon::activate(ActivationReason reason)
{
  switch (reason) {
  case Context:
    invalidateContextMenu();
    break;

  case Trigger:
    w_->show();
    break;
  case DoubleClick:
  case MiddleClick:
    w_->openDirectory();
    break;

  default: ;
  }
}

void
SystemTrayIcon::invalidateContextMenu()
{ }

// EOF

/*
void
SystemTrayIcon::setWindowStaysOnTop(bool t)
{
  if (t != w_->isWindowStaysOnTop()) {
    const char *slot = t ? SLOT(setWindowStaysOnTopToTrue())
                         : SLOT(setWindowStaysOnTopToFalse());
    QTimer::singleShot(0, this, slot);
  }
}

void
SystemTrayIcon::setWindowStaysOnTopToTrue()
{ w_->setWindowStaysOnTop(true); }

void
SystemTrayIcon::setWindowStaysOnTopToFalse()
{ w_->setWindowStaysOnTop(false); }

*/
