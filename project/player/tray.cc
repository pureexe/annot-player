// tray.cc
// 10/27/2011

#include "tray.h"
#include "rc.h"
#include "tr.h"
#include "mainwindow.h"
#include <QtGui>

// - Construction -

Tray::Tray(MainWindow *w, QObject *parent)
  : Base(QIcon(RC_IMAGE_APP), parent), w_(w)
{
  Q_ASSERT(w_);
  setToolTip(TR(T_TITLE_PROGRAM));

  createActions();

  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          SLOT(activate(QSystemTrayIcon::ActivationReason)));
}

void
Tray::createActions()
{
  connect(toggleWindowOnTopAct_ = new QAction(TR(T_MENUTEXT_WINDOWSTAYSONTOP), this),
          SIGNAL(triggered(bool)), w_, SLOT(setWindowOnTop(bool)));
  toggleWindowOnTopAct_->setCheckable(true);

  // Menu
  QMenu *menu = new QMenu(w_); {
    menu->addAction(TR(T_MENUTEXT_OPENFILE), w_, SLOT(openFile()));
    menu->addAction(TR(T_MENUTEXT_OPENURL), w_, SLOT(openUrl()));
    menu->addAction(TR(T_MENUTEXT_OPENANNOTATIONURL), w_, SLOT(openAnnotationUrl()));
    menu->addAction(TR(T_MENUTEXT_OPENDIRECTORY), w_, SLOT(openBrowsedDirectory()));
#ifdef USE_MODE_SIGNAL
    menu->addAction(TR(T_MENUTEXT_PROCESSPICKDIALOG), w_, SLOT(openWindow()));
#endif // USE_MODE_SIGNAL
#ifdef WITH_WIN_PICKER
    menu->addAction(TR(T_MENUTEXT_WINDOWPICKDIALOG), w_, SLOT(showWindowPickDialog()));
#endif // WITH_WIN_PICKER
    menu->addSeparator();
#ifndef Q_WS_MAC
    menu->addAction(toggleWindowOnTopAct_);
#endif // Q_WS_MAC
    minimizeAct_ = menu->addAction(TR(T_MINIMIZE), w_, SLOT(showMinimized()));
    restoreAct_ = menu->addAction(TR(T_RESTORE), w_, SLOT(showNormal()));
    menu->addSeparator();
    menu->addAction(TR(T_MENUTEXT_ABOUT), w_, SLOT(about()));
    menu->addAction(TR(T_MENUTEXT_HELP), w_, SLOT(help()));
    menu->addAction(TR(T_MENUTEXT_QUIT), w_, SLOT(close()));
  }
  setContextMenu(menu);
}

// - Events -

void
Tray::activate(ActivationReason reason)
{
  switch (reason) {
  case Context: updateContextMenu(); break;
  case Trigger: restoreAct_->trigger(); break;
  case DoubleClick: case MiddleClick: w_->openBrowsedDirectory(); break;
  default: ;
  }
}

void
Tray::updateContextMenu()
{
  minimizeAct_->setVisible(!w_->isMinimized());
  restoreAct_->setVisible(!w_->isVisible() || w_->isMinimized());
  toggleWindowOnTopAct_->setChecked(w_->isWindowOnTop());
}

// EOF

/*
void
Tray::setWindowStaysOnTop(bool t)
{
  if (t != w_->isWindowStaysOnTop()) {
    const char *slot = t ? SLOT(setWindowStaysOnTopToTrue())
                         : SLOT(setWindowStaysOnTopToFalse());
    QTimer::singleShot(0, this, slot);
  }
}

void
Tray::setWindowStaysOnTopToTrue()
{ w_->setWindowStaysOnTop(true); }

void
Tray::setWindowStaysOnTopToFalse()
{ w_->setWindowStaysOnTop(false); }

*/
