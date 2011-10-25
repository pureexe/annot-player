// osdwindow.cc
// 7/12/2011

#include "osdwindow.h"
#include "core/eventlistener.h"
#include <QtGui>

//#define DEBUG "OSDWindow"
#include "module/debug/debug.h"

// - OSDDock -

OSDDock::OSDDock(QWidget *parent)
  : Base(parent)
{
  setTitleBarWidget(new QWidget(this));

  setAttribute(Qt::WA_TranslucentBackground);
  setContentsMargins(0, 0, 0, 0); // full window on start
  setFeatures(QDockWidget::DockWidgetFloatable);
  setFloating(true);
  setAllowedAreas(Qt::NoDockWidgetArea);

  //setWindowOpacity(0);
  //setStyleSheet("background-color:transparent");
  //setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
}

void
OSDDock::setVisible(bool visible)
{
  if (widget())
    widget()->setVisible(visible);
  Base::setVisible(visible);
}

// FIXME: This function has no effect?!!! cannot previout closing
void
OSDDock::closeEvent(QCloseEvent *event)
{
  if (event && isVisible())
    event->ignore();
}

// - OSDWindow -

OSDWindow::OSDWindow(QWidget *parent)
  : Base(parent), listener_(0)
{
  setContentsMargins(0, 0, 0, 0);

  //poller_ = new QTimer(this);
  //connect(poller_, SIGNAL(timeout()), SLOT(repaint()));
}

Core::EventListener*
OSDWindow::eventListener() const
{ return listener_; }

void
OSDWindow::setEventListener(Core::EventListener *listener)
{ listener_ = listener; }

void OSDWindow::contextMenuEvent(QContextMenuEvent *event) { DOUT("contextMenuEvent");     if (listener_) listener_->sendContextMenuEvent(event); }
void OSDWindow::mouseMoveEvent(QMouseEvent *event)         { DOUT("mouseMoveEvent");       if (listener_) listener_->sendMouseMoveEvent(event); }
void OSDWindow::mousePressEvent(QMouseEvent *event)        { DOUT("mousePressEvent");      if (listener_) listener_->sendMousePressEvent(event); }
void OSDWindow::mouseReleaseEvent(QMouseEvent *event)      { DOUT("mouseReleaseEvent");    if (listener_) listener_->sendMouseReleaseEvent(event); }
void OSDWindow::mouseDoubleClickEvent(QMouseEvent *event)  { DOUT("mouseDoubleClickEvent");if (listener_) listener_->sendMouseDoubleClickEvent(event); }


// EOF
/*
void
OSDWindow::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  static QPoint pos(100, 100);
  QString text = tr("Love U, Jank!");

  pos += QPoint(10, 0);
  if (pos.x()>400)
    pos.setX(100);

  QPainter painter(this);
  painter.setPen(Qt::red);
  painter.setFont(QFont("Arial", 30));
  painter.drawText(pos, text);
}
*/
