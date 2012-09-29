// mainobject.cc
// 9/26/2012 jichi

#include "mainobject_p.h"
#include "mainwidget.h"

#include "librarydialog.h"

#define DEBUG "mainobject_p"
#include "qx/qxdebug.h"

// - Construction -

MainObjectPrivate::MainObjectPrivate(QObject *parent)
  : Base(parent), rootWidget_(nullptr), libraryDialog_(nullptr)
{ }

MainObjectPrivate::~MainObjectPrivate()
{
  if (rootWidget_)
    rootWidget_->deleteLater();

  foreach (QWindow *w, windows_) {
    w->hide();
    w->deleteLater();
  }
}


// - Properties -

QWidget*
MainObjectPrivate::rootWidget()
{
  if (!rootWidget_)
    rootWidget_ = new MainWidget;
  return rootWidget_;
}

QWindow*
MainObjectPrivate::libraryDialog()
{
  if (!libraryDialog_) {
    LibraryDialog *w = new LibraryDialog;
    connect(w, SIGNAL(closed()), SIGNAL(quitRequested()), Qt::QueuedConnection);
    windows_.append(w);
    libraryDialog_ = w;
  }
  return libraryDialog_;
}


// EOF
