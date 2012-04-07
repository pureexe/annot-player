// actextview.cc
// 1/1/2012

#include "ac/actextview.h"
#include "module/qtext/htmltag.h"

// - Slots -

void
AcTextView::setText(const QString &text)
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
AcTextView::moveCursorToBottom()
{
  QTextCursor tc = textCursor();
  tc.movePosition(QTextCursor::End);
  setTextCursor(tc);
}

void
AcTextView::clear()
{
  line_ = 0;
  Base::clear();
}

void
AcTextView::setText(const QStringList &l)
{
  if (l.isEmpty()) {
    clear();
    return;
  }

  QString t;
  foreach (QString s, l) {
    if (line_++ % 2)
      t.append(HTML_STYLE(s, color:purple));
    else
      t.append(HTML_STYLE(s, color:blue));
    if (line_ != l.size())
      t.append(HTML_BR());
  }

  setText(t);
}

void
AcTextView::append(const QString &text)
{
  if (!isEmpty())
    Base::append(QString());
  if (line_++ % 2)
    Base::append(HTML_STYLE(text, color:purple));
  else
    Base::append(HTML_STYLE(text, color:blue));
  moveCursorToBottom();
}

// EOF
