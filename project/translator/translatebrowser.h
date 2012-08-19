#ifndef TRANSLATEBROWSER_H
#define TRANSLATEBROWSER_H

// translatebrowser.h
// 8/13/2012

#include <QtGui/QTextBrowser>

namespace QtExt { class TextHighlighter; }

class TranslateBrowser : public QTextBrowser
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslateBrowser)
  typedef TranslateBrowser Self;
  typedef QTextBrowser Base;

  QString selectedText_;
  QtExt::TextHighlighter *highlighter_;

public:
  explicit TranslateBrowser(QWidget *parent = nullptr);

public slots:
  void highlightText(const QString &t);

signals:
  void selectedTextChanged(const QString &text);

protected slots:
  void updateSelectedText();
};

#endif // TRANSLATEBROWSER_H
