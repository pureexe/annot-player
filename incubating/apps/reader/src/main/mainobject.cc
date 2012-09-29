// mainobject.cc
// 9/26/2012 jichi

#include "mainobject.h"
#include "mainobject_p.h"
#include "qx/qxuiutil.h"
#include <QtGui/QWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#define DEBUG "mainobject"
#include "qx/qxdebug.h"

// - Construction -

MainObject::MainObject()
  : d(new D(this))
{
  connect(d, SIGNAL(quitRequested()), SLOT(quit()));
}

MainObject::~MainObject()
{ }

// - Startup -

void
MainObject::launch(const QStringList &args)
{
  DOUT("enter: args =" << args);
  Q_UNUSED(args);

  QGuiApplication::primaryScreen()->size();
  QWindow *w = d->libraryDialog();
  qxCenterWindow(w);
  w->show();
  DOUT("exit");
}

// - Close -

void
MainObject::quit()
{
  DOUT("enter");
  QApplication::quit();
  DOUT("exit");
}

// EOF
