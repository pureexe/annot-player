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
  setToolTip(tr("Annot Downloader"));

  createActions();
  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(activate(QSystemTrayIcon::ActivationReason)));
}

void
TrayIcon::createActions()
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
TrayIcon::activate(ActivationReason reason)
{
  switch (reason) {
  case Context:
    updateContextMenu();
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
TrayIcon::updateContextMenu()
{ }

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
