#ifndef USERPANEL_H
#define USERPANEL_H

// userpanel.h
// 8/3/2011

#include <QWidget>

/*
#include <QDockWidget>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QToolButton;
QT_END_NAMESPACE

class UserPanelDock : public QDockWidget
{
  Q_OBJECT
  typedef UserPanelDock Self;
  typedef QDockWidget Base;

  QPoint dragPos_;

public:
  explicit UserPanelDock(QWidget *parent = 0);

protected:
  void forward(QEvent *event);

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void contextMenuEvent(QContextMenuEvent *event);

  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dragLeaveEvent(QDragLeaveEvent *event);
  virtual void dropEvent(QDropEvent *event);

  //virtual void closeEvent(QCloseEvent *event);
  //virtual void keyPressEvent(QKeyEvent *event);
  //virtual void keyReleaseEvent(QKeyEvent *event);
};
*/

class UserPanel : public QWidget
{
  Q_OBJECT
  typedef UserPanel Self;
  typedef QWidget Base;


public:
  explicit UserPanel(QWidget *parent = 0);
};

#endif // USERPANEL_H
