// gui/wblineedit.cc
// 3/31/2012

#include "gui/wblineedit.h"
#include <QtGui>

void
WbLineEdit::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
  case Qt::Key_Escape:
    clearFocus();    // FIXME: after clear focus, which window get new focus?
    event->accept();
    return;

  case Qt::Key_Return:
    event->accept();
    emit returnPressed();
    return;

  //case Qt::Key_Up:
  //  previous();
  //  event->accept();
  //  return;
  //case Qt::Key_Down:
  //  next();
  //  event->accept();
  //  return;
  }

  Base::keyPressEvent(event);
}

// EOF
