// tray.cc
// 10/27/2011

#include "tray.h"
#include "rc.h"
#include "tr.h"
#include "mainwindow.h"
#include <QtGui>

// - Constructions -

Tray::Tray(MainWindow *w, QObject *parent)
  : Base(parent), w_(w)
{
  Q_ASSERT(w_);
  setIcon(QIcon(RC_IMAGE_APP));

  createActions();

  connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          SLOT(activate(QSystemTrayIcon::ActivationReason)));
}

void
Tray::createActions()
{
  // Actions
#define MAKE_ACTION(_action, _styleid, _receiver, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _receiver, _slot);
#define MAKE_TOGGLE(_action, _styleid, _receiver, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  _action->setCheckable(true); \
  connect(_action, SIGNAL(triggered(bool)), _receiver, _slot);

  //QAction *MAKE_ACTION(openAct,         OPENFILE,       w_,     SLOT(open()))
  QAction *MAKE_ACTION(openFileAct,     OPENFILE,       w_,     SLOT(openFile()))
  QAction *MAKE_ACTION(openUrlAct,      OPENURL,        w_,     SLOT(openUrl()))
  QAction *MAKE_ACTION(openAnnotationUrlAct,   OPENANNOTATIONURL, w_,  SLOT(openAnnotationUrl()))
  QAction *MAKE_ACTION(openWindowAct,   PROCESSPICKDIALOG,w_,   SLOT(openWindow()))
  QAction *MAKE_ACTION(pickWindowAct,   WINDOWPICKDIALOG, w_,   SLOT(showWindowPickDialog()))
  QAction *MAKE_ACTION(aboutAct,        ABOUT,          w_,     SLOT(about()))
  QAction *MAKE_ACTION(helpAct,         HELP,           w_,     SLOT(help()))
  QAction *MAKE_ACTION(quitAct,         QUIT,           w_,     SLOT(close()))
  MAKE_ACTION(minimizeAct_,     MINIMIZE,       w_,     SLOT(showMinimized()))
  MAKE_ACTION(restoreAct_,      RESTORE,        w_,      SLOT(showNormal()))

  MAKE_TOGGLE(toggleWindowOnTopAct_, WINDOWSTAYSONTOP, w_, SLOT(setWindowOnTop(bool)))

  openFileAct->setShortcuts(QKeySequence::Open);
  openUrlAct->setShortcuts(QKeySequence::New);
  helpAct->setShortcuts(QKeySequence::HelpContents);
  quitAct->setShortcuts(QKeySequence::Quit);
#undef MAKE_ACTION
#undef MAKE_TOGGLE

  // Menu
  QMenu *menu = new QMenu(w_); {
    menu->addAction(openFileAct);
    menu->addAction(openUrlAct);
    menu->addAction(openAnnotationUrlAct);
#ifdef USE_MODE_SIGNAL
    menu->addAction(openWindowAct);
#endif // USE_MODE_SIGNAL
#ifdef WITH_WIN_PICKER
    menu->addAction(pickWindowAct);
#endif // WITH_WIN_PICKER
    menu->addSeparator();
    menu->addAction(aboutAct);
    menu->addAction(helpAct);
    menu->addAction(toggleWindowOnTopAct_);
    menu->addAction(minimizeAct_);
    menu->addAction(restoreAct_);
    menu->addAction(quitAct);
  }
  setContextMenu(menu);
}

// - Events -

void
Tray::activate(ActivationReason reason)
{
  switch (reason) {
  case Context:
    invalidateContextMenu();
    break;

  case DoubleClick:
  case MiddleClick:
    restoreAct_->trigger();
    break;

  case Trigger:
  default:
    break;
  }
}

void
Tray::invalidateContextMenu()
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
