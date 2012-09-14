// mainlibraryview.cc
// 8/17/2012

#include "mainlibraryview.h"
#include <QtGui/QSizePolicy>

// - Construction -

MainLibraryView::MainLibraryView(MediaLibrary *library, QWidget *parent)
  : Base(library, parent)
{
  setDraggable(false);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize
MainLibraryView::sizeHint() const
{ return QSize(550, 400); } // 550 > 260*2

// EOF
