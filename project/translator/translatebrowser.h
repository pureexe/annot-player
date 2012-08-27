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

  QAction *translateAct_,
          *clearAct_;

public:
  explicit TranslateBrowser(QWidget *parent = nullptr);

public slots:
  void highlightText(const QString &t);

signals:
  void selectedTextChanged(const QString &text);

public:
  QVariant loadResource(int type, const QUrl &name) override;

protected slots:
  void updateSelectedText();

  void invalidateSelectedText() { emit selectedTextChanged(selectedText_); }

  void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // TRANSLATEBROWSER_H
