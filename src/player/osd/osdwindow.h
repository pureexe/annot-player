#ifndef OSDWINDOW_H
#define OSDWINDOW_H

// osdwindow.h
// 7/12/2011

#include <QtGui/QWidget>

class QxEventForwarder;

///  Provide an Osd layer
class OsdWindow : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(OsdWindow)
  typedef OsdWindow Self;
  typedef QWidget Base;

  QxEventForwarder *forwarder_;

public:
  explicit OsdWindow(QWidget *parent = nullptr);

  //QObject *eventForwarder() const { return forward; }
  void setEventForwarder(QxEventForwarder *forwarder) { forwarder_ = forwarder; }

  bool isWindowOnTop() const;
public slots:
  void showInOsdMode();
  void setWindowOnTop(bool t = true);

protected:
  void forward(QEvent *event);

  //bool event(QEvent *event) override;

  void contextMenuEvent(QContextMenuEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void closeEvent(QCloseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
};

#endif // OSDWINDOW_H
