// qxtimeedit.cc
// 5/1/2012

#include "qtx/qxtimeedit.h"
#include <QtGui/QKeyEvent>

QTX_BEGIN_NAMESPACE

// - Events -

void
QxTimeEdit::enterEvent(QEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  Base::enterEvent(event);
}

void
QxTimeEdit::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  Base::leaveEvent(event);
}

void
QxTimeEdit::keyPressEvent(QKeyEvent *event)
{
  Base::keyPressEvent(event);
  if (event->key() == Qt::Key_Return)
    emit returnPressed();
}

QTX_END_NAMESPACE
