// actextview.cc
// 1/1/2012

#include "ac/actextview.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

namespace { // anonymous
  inline QString colorToString(const QColor &c)
  { return QString().sprintf("#%02x%02x%02x",c.red(), c.green(), c.blue()); }
} // anonymous namespace

// - Slots -

void
AcTextView::setText(const QString &text, const QColor &color)
{
  if (text.isEmpty())
    clear();
  else {
    line_++;
    if (!text.trimmed().isEmpty())
      last_ = text;

    if (color.isValid())
      setHtml(html_style(text, "color:" + colorToString(color)));
    else
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
AcTextView::setText(const QStringList &l, const QColor &color)
{
  if (l.isEmpty()) {
    clear();
    return;
  }
  QString t;
  foreach (QString s, l) {
    if (!s.trimmed().isEmpty())
      last_ = s;
    if (color.isValid())
      t.append(html_style(s, "color:" + colorToString(color)));
    else if (line_++ % 2)
      t.append(HTML_STYLE(s, color:purple));
    else
      t.append(HTML_STYLE(s, color:blue));
    if (line_ != l.size())
      t.append(HTML_BR());
  }

  setText(t);
}

void
AcTextView::append(const QString &text, const QColor &color)
{
  if (!text.trimmed().isEmpty())
    last_ = text;
  if (!isEmpty())
    Base::append(QString::null);
  if (color.isValid())
    Base::append(html_style(text, "color:" + colorToString(color)));
  else if (line_++ % 2)
    Base::append(HTML_STYLE(text, color:purple));
  else
    Base::append(HTML_STYLE(text, color:blue));
  moveCursorToBottom();
}

// EOF
