// positionslider.cc
// 3/20/2012

#include "positionslider.h"
#include "stylesheet.h"
#include <QtGui>

#define RUNNING_COLOR   "orange"
#define STOPPED_COLOR   "gray"

#define SS_BASE \
  SS_BEGIN(QSlider::groove:horizontal) \
    SS_BORDER(1px) \
    SS_HEIGHT(2px) \
    SS_NO_MARGIN \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal) \
    SS_BACKGROUND( \
      qlineargradient(spread:pad, \
        x1:0, y1:1, x2:0, y2:0, \
        stop:0    rgba(27, 5, 27, 255), \
        stop:0.25 rgba(99, 20, 102, 255), \
        stop:0.5  rgba(154, 30, 158, 255), \
        stop:1    rgba(173, 57, 176, 255))) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal) \
    SS_TRANSPARENT \
    SS_MARGIN(-4px) \
    SS_WIDTH(8px) \
    SS_BORDER_RADIUS(4px) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:hover) \
    SS_BACKGROUND( \
      qlineargradient( \
        x1:0, y1:0, x2:1, y2:1, \
        stop:0 #bbf, \
        stop:1 #66e)) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_TRANSPARENT \
    SS_NO_MARGIN \
    SS_NO_WIDTH \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal:disabled) \
    SS_BACKGROUND(#bbb) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal:disabled) \
    SS_BACKGROUND(#eee) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_BACKGROUND(#eee) \
    SS_BORDER(1px solid #aaa) \
  SS_END

#define SS_STOPPED \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_BACKGROUND(#555) \
    SS_BORDER(1px solid #777) \
    SS_HEIGHT(1px) \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END

#define SS_RUNNING(_start, _stop) \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_HEIGHT(1px) \
    SS_LEFT(5px) SS_RIGHT(5px) \
    SS_BACKGROUND_BEGIN \
      "qlineargradient(spread:pad," \
        "x1:0,y1:0,x2:1,y2:0," \
        "stop:0"        " " RUNNING_COLOR "," \
        "stop:" _start  " " RUNNING_COLOR "," \
        "stop:" _stop   " " STOPPED_COLOR "," \
        "stop:1"        " " STOPPED_COLOR ")" \
    SS_STMT_END \
  SS_END

// - Construction -

PositionSlider::PositionSlider(QWidget *parent)
  : Base(Qt::Horizontal, parent), availablePosition_(0)
{
  connect(this, SIGNAL(sliderMoved(int)), SLOT(invalidateStyleSheet(int)));
  invalidateStyleSheet();
}

void
PositionSlider::setAvailablePosition(int value)
{
  if (availablePosition_ != value) {
    availablePosition_ = value;
    invalidateStyleSheet();
  }
}

// - Style sheet -

void
PositionSlider::invalidateStyleSheet(int current)
{
  if (current <= 0)
    current = sliderPosition();

  int total = maximum();
  if (total <= 0 ||
      availablePosition_ <= 0 || total <= availablePosition_ ||
      availablePosition_ <= current || total <= current) {
    setStyleSheet(SS_BASE SS_STOPPED);
    return;
  }

  qreal progress = (availablePosition_ - current) / qreal(total - current);
  if (progress <= 0 || progress >= 1) { // should never happen
    setStyleSheet(SS_BASE SS_STOPPED);
    return;
  }

  qreal threshold = qMin(0.05, progress / 2);

  QString s1 = QString::number(progress - threshold),
          s2 = QString::number(progress);
  setStyleSheet(SS_BASE SS_RUNNING(+s1+, +s2+));
}

// EOF
