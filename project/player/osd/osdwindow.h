#ifndef OSDWINDOW_H
#define OSDWINDOW_H

// osdwindow.h
// 7/12/2011

#include <QtGui/QWidget>

namespace QtExt { class EventListener; }

///  Provide an Osd layer
class OsdWindow : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(OsdWindow)
  typedef OsdWindow Self;
  typedef QWidget Base;

  QtExt::EventListener *listener_;

public:
  explicit OsdWindow(QWidget *parent = 0);

  //QObject *eventListener() const { return listener_; }
  void setEventListener(QtExt::EventListener *listener) { listener_ = listener; }

  bool isWindowOnTop() const;
public slots:
  void showInOsdMode();
  void setWindowOnTop(bool t = true);

protected:
  void forward(QEvent *event);

  //virtual bool event(QEvent *event); ///< \override

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< override
  virtual void mousePressEvent(QMouseEvent *event); ///< override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< override
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< override

  virtual void closeEvent(QCloseEvent *event); ///< \override
  virtual void resizeEvent(QResizeEvent *event); ///< \override
};

#endif // OSDWINDOW_H
