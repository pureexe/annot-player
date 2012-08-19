// mainlibraryview_dock.cc
// 8/17/2012

#include "mainlibraryview.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QContextMenuEvent>

// - Dock -

MainLibraryDock::MainLibraryDock(QWidget *parent)
  : Base(parent)
{
  setContentsMargins(0, 0, 0, 0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setTitleBarWidget(new QWidget(this));
  setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void
MainLibraryDock::contextMenuEvent(QContextMenuEvent *event)
{
  if (widget())
    QCoreApplication::sendEvent(widget(), event);
  else
    Base::contextMenuEvent(event);
}

//void
//MainLibraryDock::setVisible(bool visible)
//{
//  if (widget())
//    widget()->setVisible(visible);
//
//  Base::setVisible(visible);
//}

// EOF
