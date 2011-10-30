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
}

void
Tray::createActions()
{
  // Actions
#define MAKE_ACTION(_action, _styleid, _receiver, _slot) \
  QAction *_action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _receiver, _slot);

  MAKE_ACTION(openAct,          OPENFILE,       w_,     SLOT(open()))
  MAKE_ACTION(openFileAct,      OPENFILE,       w_,     SLOT(openFile()))
  MAKE_ACTION(openProcessAct,   SIGNALVIEW,     w_,     SLOT(openProcess()))
  MAKE_ACTION(aboutAct,         ABOUT,          w_,     SLOT(about()))
  MAKE_ACTION(helpAct,          HELP,           w_,     SLOT(help()))
  MAKE_ACTION(quitAct,          QUIT,           w_,     SLOT(close()))

  openAct->setShortcuts(QKeySequence::Open);
  helpAct->setShortcuts(QKeySequence::HelpContents);
  quitAct->setShortcuts(QKeySequence::Quit);
#undef MAKE_ACTION

  // Menu
  QMenu *menu = new QMenu(w_); {
    menu->addAction(openFileAct);
#ifdef USE_MODE_SIGNAL
    menu->addAction(openProcessAct);
#endif // USE_MODE_SIGNAL
    menu->addSeparator();
    menu->addAction(aboutAct);
    menu->addAction(helpAct);
    menu->addAction(quitAct);
  }
  setContextMenu(menu);
}

// EOF
