#ifndef TRANSLATEEDIT_H
#define TRANSLATEEDIT_H

// translateedit.h
// 8/13/2012

#include <QtGui/QTextEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

namespace QtExt { class TextHighlighter; }

class TranslateEdit : public QTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslateEdit)
  typedef TranslateEdit Self;
  typedef QTextEdit Base;

  QString selectedText_;

  QMenu *contextMenu;
  QtExt::TextHighlighter *highlighter_;

  QAction *translateAct_,
          *clearAct_;

public:
  explicit TranslateEdit(QWidget *parent = nullptr);

public slots:
  void highlightText(const QString &t);

signals:
  void selectedTextChanged(const QString &text);

protected slots:
  void updateSelectedText();
  void invalidateSelectedText() { emit selectedTextChanged(selectedText_); }

protected:
  void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // TRANSLATEEDIT_H
