#ifndef EMBEDDEDCANVAS_H
#define EMBEDDEDCANVAS_H

// embeddedcanvas.h
// 4/8/2012

#include "module/annotcloud/annotation.h"
#include <QWidget>

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

  // - Events -
protected:
  virtual void paintEvent(QPaintEvent *event); ///< \override

  // - Paint -
protected:
  void paint(QPainter &painter, const AnnotCloud::AnnotationList &l);

private:
  DataManager *data_;
  SignalHub *hub_;
  Player *player_;
};

#endif // EMBEDDEDCANVAS_H
