#ifndef _CORE_GUI_DIALOG_H
#define _CORE_GUI_DIALOG_H

// core/dialog.h
// 9/3/2011

#include <QWidget>

namespace Core { namespace Gui {

class Dialog : public QWidget
{
  Q_OBJECT
  typedef Dialog Self;
  typedef QWidget Base;

  bool draggable_;
  QPoint dragPos_;

public:
  ///  Default f: Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
  explicit Dialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

  // - Properties -
public:
  bool draggable() const;
public slots:
  void setDraggable(bool draggable);

  // - Events -
protected:
  //virtual void setVisible(bool visible); ///< \override
  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override
};

} } // namespace Gui, Core

#endif // _CORE_GUI_DIALOG_H
