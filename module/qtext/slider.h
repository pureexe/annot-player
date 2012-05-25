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

public:
  explicit Slider(QWidget *parent = 0)
    : Base(parent) { }
  explicit Slider(Qt::Orientation orientation, QWidget *parent = 0)
    : Base(orientation, parent) { }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override

  virtual void mousePressEvent(QMouseEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_SLIDER_H
