#ifndef EMBEDDEDCANVAS_H
#define EMBEDDEDCANVAS_H

// embeddedcanvas.h
// 4/8/2012

#include "module/annotcloud/annotation.h"
#include <QtGui/QWidget>

QT_FORWARD_DECLARE_CLASS(QPainter)

class DataManager;
class SignalHub;
class Player;

class EmbeddedCanvas : public QWidget
{
  Q_OBJECT
  typedef EmbeddedCanvas Self;
  typedef QWidget Base;

public:
  EmbeddedCanvas(DataManager *data, SignalHub *hub, Player *player, QWidget *parent = 0);

signals:
  void enabledChanged(bool enabled);
  void visibleChanged(bool visible);

public:
  bool isEnabled() const { return enabled_; }
  bool isEmpty() const;
public slots:
  void setEnabled(bool t);
  void setOffset(qint64 secs);
  void invalidateVisible();

  // - Events -
public slots:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \override
  virtual void setVisible(bool visible); ///< \override
protected:
  virtual void paintEvent(QPaintEvent *event); ///< \override

  // - Paint -
protected:
  void paintHistogram(QPainter &painter, const QRect &view, const AnnotCloud::AnnotationList &l);

private:
  bool enabled_;
  DataManager *data_;
  SignalHub *hub_;
  Player *player_;
  qint64 offset_;
};

#endif // EMBEDDEDCANVAS_H
