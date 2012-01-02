#ifndef TEXTVIEW_H
#define TEXTVIEW_H

// textview.h
// 1/1/2012

#include "textedit.h"
#include <QStringList>

class TextView : public TextEdit
{
  Q_OBJECT
  typedef TextView Self;
  typedef TextEdit Base;

public:
  explicit TextView(QWidget *parent = 0)
    : Base(parent), line_(0) { }

  bool isEmpty() const { return !line_; }

public slots:
  void setText(const QString &text);
  void setText(const QStringList &lines);
  void append(const QString &text);
  void clear();

  void moveCursorToBottom();

private:
  int line_;
};

#endif // TEXTVIEW_H
