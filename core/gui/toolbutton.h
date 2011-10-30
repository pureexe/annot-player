#ifndef _CORE_GUI_TOOLBUTTON_H
#define _CORE_GUI_TOOLBUTTON_H

// core/gui/toolbutton.h
// 8/6/2011

#include <QToolButton>
#include <QCursor>

namespace Core { namespace Gui {

class ToolButton : public QToolButton
{
  Q_OBJECT
  typedef ToolButton Self;
  typedef QToolButton Base;

  bool hover_;
  QCursor lastCursor_;
  QCursor hoverCursor_;

public:
  explicit ToolButton(QWidget *parent = 0);

  bool isHovered() const;

  QCursor hoverCursor() const;
  void setHoverCursor(const QCursor &cursor);

signals:
  //void hovered();
  //void leaved();

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

} } // namespace Gui, Core

#endif // _CORE_GUI_TOOLBUTTON_H
