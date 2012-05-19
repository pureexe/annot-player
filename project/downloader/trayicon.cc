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
  QMenu *m = new QMenu(w_); {
    m->addAction(tr("Add"), w_, SLOT(add()));
    m->addSeparator();
    m->addAction(tr("Show"), w_, SLOT(show()));
    m->addAction(tr("Quit"), w_, SLOT(close()));
  } setContextMenu(m);
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
