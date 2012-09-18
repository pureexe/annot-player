// qxdatetimeedit.cc
// 5/1/2012

#include "qtx/qxdatetimeedit.h"
#include <QtGui/QKeyEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxDateTimeEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxDateTimeEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QxDateTimeEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

QTX_END_NAMESPACE
