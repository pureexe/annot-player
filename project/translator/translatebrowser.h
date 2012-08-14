#ifndef TRANSLATEBROWSER_H
#define TRANSLATEBROWSER_H

// translatebrowser.h
// 8/13/2012

#include <QtGui/QTextBrowser>

class TranslateBrowser : public QTextBrowser
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslateBrowser)
  typedef TranslateBrowser Self;
  typedef QTextBrowser Base;

  QString selectedText_;

public:
  explicit TranslateBrowser(QWidget *parent = nullptr);

signals:
  void selectedTextChanged(const QString &text);

protected slots:
  void updateSelectedText();
};

#endif // TRANSLATEBROWSER_H
