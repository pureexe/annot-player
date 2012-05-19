#ifndef _QTEXT_TOOLBUTTON_H
#define _QTEXT_TOOLBUTTON_H

// qtext/toolbutton.h
// 8/6/2011

#include <QtGui/QToolButton>
#include <QtGui/QCursor>

namespace QtExt {

class ToolButton : public QToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(ToolButton)
  typedef ToolButton Self;
  typedef QToolButton Base;

public:
  explicit ToolButton(QWidget *parent = 0)
    : Base(parent) { }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_TOOLBUTTON_H
