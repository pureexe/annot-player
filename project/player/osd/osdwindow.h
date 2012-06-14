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

  //virtual bool event(QEvent *event); ///< \reimp

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \reimp
  virtual void mousePressEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \reimp

  virtual void closeEvent(QCloseEvent *event); ///< \reimp
  virtual void resizeEvent(QResizeEvent *event); ///< \reimp
};

#endif // OSDWINDOW_H
