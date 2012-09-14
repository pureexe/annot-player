// userlabel.cc
// 8/1/2011

#include "userlabel.h"
#include <QtGui>

void
UserLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    emit showUserPanelRequested();
    event->accept();
  }
}

// EOF
