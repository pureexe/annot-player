#ifndef TRANSLATEEDIT_H
#define TRANSLATEEDIT_H

// translateedit.h
// 8/13/2012

#include <QtGui/QTextEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

class TranslateEdit : public QTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslateEdit)
  typedef TranslateEdit Self;
  typedef QTextEdit Base;

  QString selectedText_;

  QMenu *contextMenu;

public:
  explicit TranslateEdit(QWidget *parent = nullptr);

signals:
  void selectedTextChanged(const QString &text);

protected slots:
  void updateSelectedText();

//protected:
//  void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // TRANSLATEEDIT_H
