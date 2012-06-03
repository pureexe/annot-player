// osdwindow.cc
// 7/12/2011

#include "osdwindow.h"
#include "module/qtext/eventlistener.h"
#ifdef Q_WS_X11
#  include "unix/qtx/qtx.h"
#endif // Q_WS_X11
#include <QtGui>

//#define DEBUG "osdwindow"
#include "module/debug/debug.h"

// - Constructions -

#define WINDOW_FLAGS_BASE \
  Qt::SplashScreen | \
  Qt::FramelessWindowHint | \
  Qt::CustomizeWindowHint

#ifdef Q_WS_MAC
#  define WINDOW_FLAGS WINDOW_FLAGS_BASE //| Qt::WindowStaysOnTopHint
#else
#  define WINDOW_FLAGS WINDOW_FLAGS_BASE
#endif // Q_WS_MA


OsdWindow::OsdWindow(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), listener_(0)
{
  setContentsMargins(0, 0, 0, 0);
  setAttribute(Qt::WA_TranslucentBackground);
  //setAttribute(Qt::WA_MacNoClickThrough);

  setMouseTracking(true);

  // FIXME: As a trade off, dragging annot etc does not work for mouse event anymore
  // Need a machanism similar to global hook.
#ifdef Q_WS_X11
  //setAttribute(Qt::WA_TransparentForMouseEvents);
  //setAttribute(Qt::WA_X11DoNotAcceptFocus);
  //setFocusPolicy(Qt::NoFocus);
#endif // Q_WS_X11

  //poller_ = new QTimer(this);
  //connect(poller_, SIGNAL(timeout()), SLOT(repaint()));
}

bool
OsdWindow::isWindowOnTop() const
{ return windowFlags() & Qt::WindowStaysOnTopHint; }

void
OsdWindow::setWindowOnTop(bool t)
{
  DOUT("enter: t =" << t);
  if (t != isWindowOnTop())
    setWindowFlags(t ? windowFlags() | Qt::WindowStaysOnTopHint :
                       windowFlags() & ~Qt::WindowStaysOnTopHint);
  if (!isVisible())
    show();
  raise();
  DOUT("exit");
}

// - Events -

void OsdWindow::contextMenuEvent(QContextMenuEvent *event) { DOUT("enter"); if (listener_) listener_->sendContextMenuEvent(event);      DOUT("exit"); }
void OsdWindow::mouseMoveEvent(QMouseEvent *event)         { DOUT("enter"); if (listener_) listener_->sendMouseMoveEvent(event);        DOUT("exit"); }
void OsdWindow::mousePressEvent(QMouseEvent *event)        { DOUT("enter"); if (listener_) listener_->sendMousePressEvent(event);       DOUT("exit"); }
void OsdWindow::mouseReleaseEvent(QMouseEvent *event)      { DOUT("enter"); if (listener_) listener_->sendMouseReleaseEvent(event);     DOUT("exit"); }
void OsdWindow::mouseDoubleClickEvent(QMouseEvent *event)  { DOUT("enter"); if (listener_) listener_->sendMouseDoubleClickEvent(event); DOUT("exit"); }

//void OsdWindow::contextMenuEvent(QContextMenuEvent *event) { if (listener_) QCoreApplication::sendEvent(listener_, event); }
//void OsdWindow::mouseMoveEvent(QMouseEvent *event)         { if (listener_) QCoreApplication::sendEvent(listener_, event); }
//void OsdWindow::mousePressEvent(QMouseEvent *event)        { if (listener_) QCoreApplication::sendEvent(listener_, event); }
//void OsdWindow::mouseReleaseEvent(QMouseEvent *event)      { if (listener_) QCoreApplication::sendEvent(listener_, event); }
//void OsdWindow::mouseDoubleClickEvent(QMouseEvent *event)  { if (listener_) QCoreApplication::sendEvent(listener_, event); }

void
OsdWindow::closeEvent(QCloseEvent *event)
{
  if (event && isVisible())
    event->ignore();
}

void
OsdWindow::resizeEvent(QResizeEvent *event)
{
  Base::resizeEvent(event);
#ifdef Q_WS_X11
  QtX::zeroWindowInputShape(winId());
#endif // Q_WS_X11
}

// - OSD -

void
OsdWindow::showInOsdMode()
{
#ifdef Q_WS_WIN
  showFullScreen();
#else
  QRect fullScreen =
#ifdef Q_WS_MAC
    QApplication::desktop()->screenGeometry(this)
#else
    QApplication::desktop()->availableGeometry(this)
#endif // Q_WS_MAC
  ;
  setGeometry(fullScreen);
  move(0, 0); // make sure it is sitting at (0, 0)
  show();
#endif // Q_WS_WIN
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

