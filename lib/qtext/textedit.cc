// qtext/textedit.cc
// 6/2/2011

#include "lib/qtext/textedit.h"
#include <QtGui/QTextCursor>

// - Actions -

// See: http://www.qtforum.org/article/32641/qtextedit-clear-the-last-line.html
void
QtExt::
TextEdit::removeLastLine()
{
  moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
  moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
  moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
  textCursor().removeSelectedText();
  textCursor().deletePreviousChar();
}

// EOF
