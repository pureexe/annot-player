// qxdateedit.cc
// 5/1/2012

#include "qtx/qxdateedit.h"
#include <QtGui/QKeyEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxDateEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxDateEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QxDateEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

QTX_END_NAMESPACE
