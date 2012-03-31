#ifndef _AC_ACTEXTVIEW_H
#define _AC_ACTEXTVIEW_H

// ac/actextview.h
// 1/1/2012

#include "ac/actextedit.h"
#include <QStringList>

class AcTextView : public AcTextEdit
{
  Q_OBJECT
  typedef AcTextView Self;
  typedef AcTextEdit Base;

public:
  explicit AcTextView(QWidget *parent = 0)
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

#endif // _AC_ACTEXTVIEW_H
