// userpanel.cc
// 8/3/2011

#include "userpanel.h"
#include "uistyle.h"
#include "tr.h"
#include "global.h"
#include "stylesheet.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

// - Panel -
UserPanel::UserPanel(QWidget *parent)
  : Base(parent)
{ }


// EOF

/*
// - Dock -

UserPanelDock::UserPanelDock(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{
  setWindowTitle(TR(T_TITLE_USER));
  //setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
  setFeatures(QDockWidget::DockWidgetFloatable);
  setFloating(true);

  UiStyle::globalInstance()->setWindowStyle(this);

  //setTitleBarWidget(new QWidget(this));
  //setWindowFlags(Qt::FramelessWindowHint);
  setAllowedAreas(Qt::NoDockWidgetArea);
  setContentsMargins(0, 0, 0, 0);
}

// ++ Events ++
void
UserPanelDock::mousePressEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton
      && dragPos_ == BAD_POS) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void
UserPanelDock::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
UserPanelDock::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = BAD_POS;
  if (event)
    event->accept();
}

void
UserPanelDock::forward(QEvent *event)
{
  // Forward event to parent
  Q_ASSERT(event);
  if (parent())
    QApplication::sendEvent(parent(), event);
}

void UserPanelDock::mouseDoubleClickEvent(QMouseEvent *event)   { forward(event); }
void UserPanelDock::contextMenuEvent(QContextMenuEvent *event)  { forward(event); }
void UserPanelDock::dragEnterEvent(QDragEnterEvent *event)      { forward(event); }
void UserPanelDock::dragMoveEvent(QDragMoveEvent *event)        { forward(event); }
void UserPanelDock::dragLeaveEvent(QDragLeaveEvent *event)      { forward(event); }
void UserPanelDock::dropEvent(QDropEvent *event)                { forward(event); }
//void UserPanelDock::keyPressEvent(QKeyEvent *event)            { forward(event); }
//void UserPanelDock::keyReleaseEvent(QKeyEvent *event)          { forward(event); }
*/
