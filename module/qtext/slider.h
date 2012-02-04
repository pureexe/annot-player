#ifndef _QTEXT_SLIDER_H
#define _QTEXT_SLIDER_H

// qtext/slider.h
// 8/6/2011

#include <QSlider>
#include <QCursor>

namespace QtExt {

class Slider : public QSlider
{
  Q_OBJECT
  typedef Slider Self;
  typedef QSlider Base;

  bool hover_;
  QCursor hoverCursor_;
  QCursor lastCursor_;

public:
  explicit Slider(QWidget *parent = 0);
  explicit Slider(Qt::Orientation orientation, QWidget *parent = 0);

  bool isHovered() const;

  QCursor hoverCursor() const;
  void setHoverCursor(const QCursor &cursor);

signals:
  //void hovered();
  //void leaved();

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override

  virtual void mousePressEvent(QMouseEvent *event); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_SLIDER_H
