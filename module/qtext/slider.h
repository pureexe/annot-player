#ifndef _QTEXT_SLIDER_H
#define _QTEXT_SLIDER_H

// qtext/slider.h
// 8/6/2011

#include <QtGui/QSlider>

namespace QtExt {

class Slider : public QSlider
{
  Q_OBJECT
  Q_DISABLE_COPY(Slider)
  typedef Slider Self;
  typedef QSlider Base;

  //int hoverValue_;

public:
  explicit Slider(QWidget *parent = 0)
    : Base(parent) { }
  explicit Slider(Qt::Orientation orientation, QWidget *parent = 0)
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
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override

  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  //virtual void mouseMoveEvent(QMouseEvent *event); // NEVER TRIGGERED orz
};

} // namespace QtExt

#endif // _QTEXT_SLIDER_H
