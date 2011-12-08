#ifndef _CORE_GUI_SLIDER_H
#define _CORE_GUI_SLIDER_H

// core/gui/slider.h
// 8/6/2011

#include <QSlider>
#include <QCursor>

namespace Core { namespace Gui {

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

} } // namespace Core, Gui

#endif // _CORE_GUI_SLIDER_H
