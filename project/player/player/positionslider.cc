// positionslider.cc
// 3/20/2012

#include "positionslider.h"
#include "stylesheet.h"
#include <QtGui>

//#define RUNNING_COLOR "rgba(0,255,255,200)"   // cyan = #00ffff, see: http://ja.wikipedia.org/wiki/シアン_(色)
//#define RUNNING_COLOR "rgba(255,0,0,200)"     // red
//#define RUNNING_COLOR "rgba(190,190,190,200)" // gray = rgb(190,190,190), see: http://www.tayloredmktg.com/rgb/
#define RUNNING_COLOR   "rgba(255,165,0,200)"   // orange = rgb(255,165,0), see: http://www.tayloredmktg.com/rgb/
#define STOPPED_COLOR   "transparent"

#define SS_BASE \
  SS_BEGIN(QSlider::groove:horizontal) \
    SS_BORDER(1px) \
    SS_HEIGHT(4px) \
    SS_NO_MARGIN \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal) \
    SS_BACKGROUND( \
      qlineargradient(spread:pad, \
        x1:0, y1:1, x2:0, y2:0, \
        stop:0    rgba(27, 5, 27, 200), \
        stop:0.25 rgba(99, 20, 102, 200), \
        stop:0.5  rgba(154, 30, 158, 200), \
        stop:1    rgba(173, 57, 176, 200))) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
    SS_NO_WIDTH \
    SS_NO_MARGIN \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal:disabled) \
    SS_BACKGROUND(#aaa) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal:disabled) \
    SS_TRANSPARENT \
    SS_BORDER_COLOR(#999) \
  SS_END

//  SS_BEGIN(QSlider::handle:horizontal:hover)
//    SS_BACKGROUND(
//      qlineargradient(
//        x1:0, y1:0, x2:1, y2:1,
//        stop:0 #bbf,
//        stop:1 #66e))
//  SS_END

#define SS_NO_ADDPAGE \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_BORDER(1px solid #777) \
    SS_LEFT(5px) SS_RIGHT(5px) \
    SS_TRANSPARENT \
  SS_END

#define SS_ADDPAGE(_start, _stop) \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_LEFT(5px) SS_RIGHT(5px) \
    SS_BACKGROUND_BEGIN \
      "qlineargradient(spread:pad," \
        "x1:0,y1:0,x2:1,y2:0," \
        "stop:0"        " " "transparent" "," \
        "stop:" _start  " " RUNNING_COLOR "," \
        "stop:" _stop   " " STOPPED_COLOR "," \
        "stop:1"        " " STOPPED_COLOR ")" \
    SS_STMT_END \
  SS_END
  //SS_BORDER(1px solid rgb(119,119,119))

#define SS_NO_HANDLE \
  SS_BEGIN(QSlider::handle:horizontal) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
    SS_NO_WIDTH \
    SS_NO_MARGIN \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:hover) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
    SS_NO_WIDTH \
    SS_NO_MARGIN \
  SS_END

#define SS_HANDLE(_width, _radius, _left) \
  SS_BEGIN(QSlider::handle:horizontal) \
    "width:" _width "px" ";" \
    "border-radius:" _radius "px" ";" \
    "margin-left:" _left "px" ";" \
    SS_MARGIN_TOP(-4px) SS_MARGIN_BOTTOM(-4px) \
    SS_BACKGROUND( \
      qradialgradient( \
        cx:0.5, cy:0.5, fx:0.5, fy:0.5, radius:1 \
        stop:0    transparent, \
        stop:0.25 transparent, \
        stop:0.3  white, \
        stop:0.45 white, \
        stop:0.5  gray)) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:hover) \
    SS_BACKGROUND( \
      qradialgradient( \
        cx:0.5, cy:0.5, fx:0.5, fy:0.5, radius:1 \
        stop:0    red, \
        stop:0.25 red, \
        stop:0.3  white, \
        stop:0.45 white, \
        stop:0.5  gray)) \
  SS_END

#define SS_GROOVE(_height) \
  SS_BEGIN(QSlider::groove:horizontal) \
    SS_BORDER(1px) \
    "height:" _height "px" ";" \
    SS_NO_MARGIN \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \

#define GROOVE_HEIGHT_MIN       4.0
#define GROOVE_HEIGHT_MAX       8.0

#define HANDLE_RADIUS_MIN       0.0
#define HANDLE_RADIUS_MAX       18.0

// - Construction -

PositionSlider::PositionSlider(QWidget *parent)
  : Base(Qt::Horizontal, parent), availablePosition_(0),
    grooveHeight_(GROOVE_HEIGHT_MIN), handleRadius_(HANDLE_RADIUS_MIN)
{
  setContentsMargins(0, 0, 0, 0);
  connect(this, SIGNAL(sliderMoved(int)), SLOT(updateStyleSheet(int)));

  grooveHeightAni_ = new QPropertyAnimation(this, "grooveHeight", this);
  grooveHeightAni_->setDuration(400); // msecs
  handleRadiusAni_ = new QPropertyAnimation(this, "handleRadius", this);
  handleRadiusAni_->setDuration(400); // msecs

  updateStyleSheet();
}

// - Properties -

void
PositionSlider::setAvailablePosition(int value)
{
  if (availablePosition_ != value) {
    availablePosition_ = value;
    updateStyleSheet();
  }
}

// - Properties -

void
PositionSlider::setGrooveHeight(qreal value)
{
  if (!qFuzzyCompare(grooveHeight_, value)) {
    grooveHeight_ = value;
    updateStyleSheet();
  }
}

void
PositionSlider::setHandleRadius(qreal value)
{
  if (!qFuzzyCompare(handleRadius_, value)) {
    handleRadius_ = value;
    updateStyleSheet();
  }
}

void
PositionSlider::updateStyleSheet(int current)
{
  if (current <= 0)
    current = sliderPosition();

  QString groove = QString::number(grooveHeight_, 'f');
  QString handleWidth = QString::number(handleRadius_, 'f'),
          handleRadius = QString::number(handleRadius_/3 +1, 'f'),
          handleLeft = QString::number(-handleRadius_/4 +1, 'f');

  int total = maximum();
  if (total <= 0 ||
      availablePosition_ <= 0 || total <= availablePosition_ ||
      availablePosition_ <= current || total <= current) {
    if (isHovered() && isEnabled())
      setStyleSheet(SS_BASE SS_HANDLE(+handleWidth+,+handleRadius+,+handleLeft+) SS_NO_ADDPAGE SS_GROOVE(+groove+));
    else
      setStyleSheet(SS_BASE SS_NO_HANDLE SS_NO_ADDPAGE SS_GROOVE(+groove+));
    return;
  }

  qreal progress = (availablePosition_ - current) / qreal(total - current);
  if (progress <= 0 || progress >= 1) { // should never happen
    if (isHovered() && isEnabled())
      setStyleSheet(SS_BASE SS_HANDLE(+handleWidth+,+handleRadius+,+handleLeft+) SS_NO_ADDPAGE SS_GROOVE(+groove+));
    else
      setStyleSheet(SS_BASE SS_NO_HANDLE SS_NO_ADDPAGE SS_GROOVE(+groove+));
    return;
  }

  qreal threshold = qMin(0.0001, progress / 100.0);

  QString s1 = QString::number(progress - threshold),
          s2 = QString::number(progress);
  if (isHovered() && isEnabled())
    setStyleSheet(SS_BASE SS_HANDLE(+handleWidth+,+handleRadius+,+handleLeft+) SS_ADDPAGE(+s1+, +s2+) SS_GROOVE(+groove+));
  else
    setStyleSheet(SS_BASE SS_NO_HANDLE SS_ADDPAGE(+s1+, +s2+) SS_GROOVE(+groove+));
}

// - Events -

void
PositionSlider::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::EnabledChange)
    updateStyleSheet();
  Base::changeEvent(event);
}


void
PositionSlider::enterEvent(QEvent *event)
{
  if (isEnabled()) {
    if (grooveHeightAni_->state() != QAbstractAnimation::Stopped)
      grooveHeightAni_->stop();
    grooveHeightAni_->setStartValue(grooveHeight_);
    grooveHeightAni_->setEndValue(GROOVE_HEIGHT_MAX);
    grooveHeightAni_->start();
    handleRadiusAni_->setStartValue(handleRadius_);
    handleRadiusAni_->setEndValue(HANDLE_RADIUS_MAX);
    handleRadiusAni_->start();
  }
  Base::enterEvent(event);
}

void
PositionSlider::leaveEvent(QEvent *event)
{
  if (isEnabled()) {
    if (grooveHeightAni_->state() != QAbstractAnimation::Stopped)
      grooveHeightAni_->stop();
    grooveHeightAni_->setStartValue(grooveHeight_);
    grooveHeightAni_->setEndValue(GROOVE_HEIGHT_MIN);

    handleRadiusAni_->setStartValue(handleRadius_);
    handleRadiusAni_->setEndValue(HANDLE_RADIUS_MIN);

    QTimer::singleShot(800, grooveHeightAni_, SLOT(start()));
    QTimer::singleShot(800, handleRadiusAni_, SLOT(start()));
  }
  Base::leaveEvent(event);
}


// EOF
