#ifndef QXSLIDER_H
#define QXSLIDER_H

// qxslider.h
// 8/6/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QSlider>

QTX_BEGIN_NAMESPACE

class QxSlider : public QSlider
{
  Q_OBJECT
  Q_DISABLE_COPY(QxSlider)
  typedef QxSlider Self;
  typedef QSlider Base;

  //int hoverValue_;

public:
  explicit QxSlider(QWidget *parent = nullptr)
    : Base(parent) { }
  explicit QxSlider(Qt::Orientation orientation, QWidget *parent = nullptr)
    : Base(orientation, parent) { }

//signals:
//  void hoverValueChanged(int value);
//
//public:
//  bool isHovered() const { return hoverValue_ >= 0; }
//  int hoverValue() const { return hoverValue_; }
//
//  void setHoverValue(int value)
//  { if (hoverValue_ != value) emit hoverValueChanged(hoverValue_ = value); }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  //void mouseMoveEvent(QMouseEvent *event) override; // NEVER TRIGGERED orz
};

QTX_END_NAMESPACE

#endif // QXSLIDER_H

