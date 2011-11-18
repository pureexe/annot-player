#ifndef OSDWINDOW_H
#define OSDWINDOW_H

// osdwindow.h
// 7/12/2011

#include <QWidget>

namespace Core { class EventListener; }

///  Provide an Osd layer
class OsdWindow : public QWidget
{
  Q_OBJECT
  typedef OsdWindow Self;
  typedef QWidget Base;

  Core::EventListener *listener_;

public:
  explicit OsdWindow(QWidget *parent = 0);

  Core::EventListener *eventListener() const;
  void setEventListener(Core::EventListener *listener);

public slots:
  void showInOsdMode();

protected:
  void forward(QEvent *event);

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< override
  virtual void mousePressEvent(QMouseEvent *event); ///< override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< override
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< override

  virtual void closeEvent(QCloseEvent *event); ///< override
};

#endif // OSDWINDOW_H
