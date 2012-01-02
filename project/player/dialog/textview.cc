// textview.cc
// 1/1/2012

#include "textview.h"
#include "textedit.h"
#include "uistyle.h"
#include "stylesheet.h"
#include "tr.h"
#include "core/gui/toolbutton.h"
#include "core/htmltag.h"
#include <QtGui>

//#define DEBUG "textview"
#include "module/debug/debug.h"

// - Slots -

void
TextView::setText(const QString &text)
{
  if (text.isEmpty())
    clear();
  else {
    line_++;
    setHtml(text);
    moveCursorToBottom();
  }
}

void
TextView::moveCursorToBottom()
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::End);
  setTextCursor(tc);
}

void
TextView::clear()
{
  line_ = 0;
  Base::clear();
}

void
TextView::setText(const QStringList &l)
{
  if (l.isEmpty()) {
    clear();
    return;
  }

  QString t;
  foreach (QString s, l) {
    if (line_++ % 2)
      t.append(CORE_HTML_STYLE(s, color:purple));
    else
      t.append(CORE_HTML_STYLE(s, color:blue));
    if (line_ != l.size())
      t.append(CORE_HTML_BR());
  }

  setText(t);
}

void
TextView::append(const QString &text)
{
  if (!isEmpty())
    Base::append(QString());
  if (line_++ % 2)
    Base::append(CORE_HTML_STYLE(text, color:purple));
  else
    Base::append(CORE_HTML_STYLE(text, color:blue));
  moveCursorToBottom();
}

// EOF
