// actextview.cc
// 1/1/2012

#include "src/common/actextview.h"
#include "htmlutil/htmltags.h"
#include <QtGui>

namespace { namespace detail {
  inline QString colorToString(const QColor &c)
  { return QString().sprintf("#%02x%02x%02x",c.red(), c.green(), c.blue()); }
} } // anonymous detail

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
      setHtml(html_ss(text, "color:" + detail::colorToString(color)));
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
  foreach (const QString &s, l) {
    if (!s.trimmed().isEmpty())
      last_ = s;
    if (color.isValid())
      t.append(html_ss(s, "color:" + detail::colorToString(color)));
    else if (line_++ % 2)
      t.append(HTML_SS(+s+, color:purple));
    else
      t.append(HTML_SS(+s+, color:blue));
    if (line_ != l.size())
      t.append(QString());
  }

  setText(t);
}

void
AcTextView::append(const QString &text, const QColor &color)
{
  if (!text.trimmed().isEmpty())
    last_ = text;
  if (!isEmpty())
    Base::append(QString());
  if (color.isValid())
    Base::append(html_ss(text, "color:" + detail::colorToString(color)));
  else if (line_++ % 2)
    Base::append(HTML_SS(+text+, color:purple));
  else
    Base::append(HTML_SS(+text+, color:blue));
  moveCursorToBottom();
}

// EOF
