#ifndef TEXTEDIT_H
#define TEXTEDIT_H

// textedit.h
// 10/10/2011

#include <QTextEdit>

QT_FORWARD_DECLARE_CLASS(QMenu)

class TextEdit : public QTextEdit
{
  Q_OBJECT
  typedef TextEdit Self;
  typedef QTextEdit Base;

public:
  explicit TextEdit(QWidget *parent = 0);

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  QMenu *contextMenu_;
};

#endif // TEXTEDIT_H
