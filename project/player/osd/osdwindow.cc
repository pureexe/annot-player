// osdwindow.cc
// 7/12/2011

#include "osdwindow.h"
#include "core/eventlistener.h"
#include <QtGui>

#define DEBUG "OsdWindow"
#include "module/debug/debug.h"

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::SplashScreen | \
  Qt::FramelessWindowHint | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint )

OsdWindow::OsdWindow(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), listener_(0)
{
  setContentsMargins(0, 0, 0, 0);

  setAttribute(Qt::WA_TranslucentBackground);
  //setAttribute(Qt::WA_MacNoClickThrough);

  //poller_ = new QTimer(this);
  //connect(poller_, SIGNAL(timeout()), SLOT(repaint()));
}

Core::EventListener*
OsdWindow::eventListener() const
{ return listener_; }

void
OsdWindow::setEventListener(Core::EventListener *listener)
{ listener_ = listener; }

// - Events -

void OsdWindow::contextMenuEvent(QContextMenuEvent *event) { DOUT("contextMenuEvent");     if (listener_) listener_->sendContextMenuEvent(event); }
void OsdWindow::mouseMoveEvent(QMouseEvent *event)         { DOUT("mouseMoveEvent");       if (listener_) listener_->sendMouseMoveEvent(event); }
void OsdWindow::mousePressEvent(QMouseEvent *event)        { DOUT("mousePressEvent");      if (listener_) listener_->sendMousePressEvent(event); }
void OsdWindow::mouseReleaseEvent(QMouseEvent *event)      { DOUT("mouseReleaseEvent");    if (listener_) listener_->sendMouseReleaseEvent(event); }
void OsdWindow::mouseDoubleClickEvent(QMouseEvent *event)  { DOUT("mouseDoubleClickEvent");if (listener_) listener_->sendMouseDoubleClickEvent(event); }

void
OsdWindow::closeEvent(QCloseEvent *event)
{
  if (event && isVisible())
    event->ignore();
}

// - OSD -
void
OsdWindow::showInOsdMode()
{
#ifdef Q_WS_MAC
  QRect fullScreen = QApplication::desktop()->screenGeometry(this);
  setGeometry(fullScreen);
  move(0, 0); // make sure it is sitting at (0, 0)
  show();

#else
  showFullScreen();
#endif // Q_WS_MAC

}

// EOF

/*
void
OsdWindow::paintEvent(QPaintEvent *event)
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

// - OsdDock -

/*
OsdDock::OsdDock(QWidget *parent)
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
OsdDock::setVisible(bool visible)
{
  if (widget())
    widget()->setVisible(visible);
  Base::setVisible(visible);
}

// FIXME: This function has no effect?!!! cannot previout closing
void
OsdDock::closeEvent(QCloseEvent *event)
{
  if (event && isVisible())
    event->ignore();
}
*/

