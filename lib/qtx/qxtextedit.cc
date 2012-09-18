// qxtextedit.cc
// 6/2/2011

#include "qtx/qxtextedit.h"
#include <QtGui/QTextCursor>

QTX_BEGIN_NAMESPACE

// - Actions -

// See: http://www.qtforum.org/article/32641/qtextedit-clear-the-last-line.html
void
QxTextEdit::removeLastLine()
{
  moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
  moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
  textCursor().removeSelectedText();
  textCursor().deletePreviousChar();
}

QTX_END_NAMESPACE
