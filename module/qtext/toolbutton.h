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
  typedef ToolButton Self;
  typedef QToolButton Base;

  bool hovered_;
  QCursor hoverCursor_,
          lastCursor_;

public:
  explicit ToolButton(QWidget *parent = 0)
    : Base(parent), hovered_(false), hoverCursor_(Qt::PointingHandCursor) { }

  bool isHovered() const { return hovered_; }

  QCursor hoverCursor() const { return hoverCursor_; }
public slots:
  void setHoverCursor(const QCursor &cursor) { hoverCursor_ = cursor; }

signals:
  void hovered();
  void leaved();

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_TOOLBUTTON_H
