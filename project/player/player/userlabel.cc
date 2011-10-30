// userlabel.cc
// 8/1/2011

#include "userlabel.h"
#include <QtGui>

UserLabel::UserLabel(QWidget *parent)
  : Base(parent) { }

void
UserLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    emit showUserPanelRequested();
    event->accept();
  }
}

// EOF
