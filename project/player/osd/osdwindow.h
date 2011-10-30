#ifndef OSDWINDOW_H
#define OSDWINDOW_H

// osdwindow.h
// 7/12/2011

#include <QWidget>
#include <QDockWidget>

namespace Core { class EventListener; }

// Dock widget for OSDWindow
class OSDDock : public QDockWidget
{
  Q_OBJECT
  typedef OSDDock Self;
  typedef QDockWidget Base;

public:
  explicit OSDDock(QWidget *parent = 0);

  virtual void setVisible(bool visible); // stop polling when hidden
protected:
  virtual void closeEvent(QCloseEvent *event); ///< override
};

///  Provide an OSD layer
class OSDWindow : public QWidget
{
  Q_OBJECT
  typedef OSDWindow Self;
  typedef QWidget Base;

  Core::EventListener *listener_;

public:
  explicit OSDWindow(QWidget *parent = 0);

  Core::EventListener *eventListener() const;
  void setEventListener(Core::EventListener *listener);

protected:
  void forward(QEvent *event);

  virtual void contextMenuEvent(QContextMenuEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // OSDWINDOW_H
