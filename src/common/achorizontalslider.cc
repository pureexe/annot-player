// achorizontalslider.cc
// 5/16/2012
// HTML color names: http://www.w3schools.com/html/html_colornames.asp

#include "src/common/achorizontalslider.h"
#include "src/common/acui.h"
#include "lib/qtext/ss.h"
#include <QtCore/QEvent>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>

// - Stylesheets -

// See: http://thesmithfam.org/blog/2009/09/17/qt-stylesheets-button-bar-tutorial/
// See: http://blog.sina.com.cn/s/blog_791f544a0100s2ml.html
//#define SS_SLIDER "QSlider::groove:horizontal { border: 1px solid #bbb; background: white; height: 10px; border-radius: 4px; } QSlider::sub-page:horizontal { background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #66e, stop: 1 #bbf); background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #bbf, stop: 1 #55f); border: 1px solid #777; height: 10px; border-radius: 4px; } QSlider::add-page:horizontal { background: #fff; border: 1px solid #777; height: 10px; border-radius: 4px; } QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc); border: 1px solid #777; width: 13px; margin-top: -2px; margin-bottom: -2px; border-radius: 4px; } QSlider::handle:horizontal:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fff, stop:1 #ddd); border: 1px solid #444; border-radius: 4px; } QSlider::sub-page:horizontal:disabled { background: #bbb; border-color: #999; } QSlider::add-page:horizontal:disabled { background: #eee; border-color: #999; } QSlider::handle:horizontal:disabled { background: #eee; border: 1px solid #aaa; border-radius: 4px; } "
#define SS_BASE \
  SS_BEGIN(QSlider::sub-page:horizontal) \
    SS_BACKGROUND( \
      qlineargradient(spread:pad, \
        x1:0, y1:1, x2:0, y2:0, \
        stop:0    rgba(27,5,27,200), \
        stop:0.25 rgba(99,20,102,200), \
        stop:0.5  rgba(154,30,158,200), \
        stop:1    rgba(173,57,176,200))) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_BACKGROUND_COLOR(rgba(255,0,0,100)) \
    SS_BORDER(1px solid #777) \
    SS_HEIGHT(1px) \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal) \
    SS_MARGIN_TOP(-3px) SS_MARGIN_BOTTOM(-3px) \
    SS_MARGIN_LEFT(-2px) \
    SS_WIDTH(5px) \
    SS_BORDER_RADIUS(2px) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
    SS_NO_WIDTH \
    SS_NO_MARGIN \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal:disabled) \
    SS_BACKGROUND(#bbb) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal:disabled) \
    SS_BACKGROUND(#eee) \
    SS_BORDER_COLOR(#999) \
  SS_END

// SS_BEGIN(QSlider::handle:horizontal:hover)
//   SS_BACKGROUND(
//     qlineargradient(
//       x1:0, y1:0, x2:1, y2:1,
//       stop:0 #bbf,
//       stop:1 #66e))
// SS_END

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

#define SS_HANDLE \
  SS_BEGIN(QSlider::handle:horizontal) \
    SS_BACKGROUND( \
      qradialgradient( \
        cx:0.5, cy:0.5, fx:0.5, fy:0.5, radius:1 \
        stop:0    transparent, \
        stop:0.25 transparent, \
        stop:0.3  white, \
        stop:0.5  white)) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:hover) \
    SS_BACKGROUND( \
      qradialgradient( \
        cx:0.5, cy:0.5, fx:0.5, fy:0.5, radius:1 \
        stop:0    red, \
        stop:0.25 red, \
        stop:0.3  white, \
        stop:0.5  white)) \
  SS_END

#define SS_GROOVE(_height) \
  SS_BEGIN(QSlider::groove:horizontal) \
    SS_BORDER(1px) \
    "height:" _height "px" ";" \
    SS_NO_MARGIN \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \

#define GROOVE_HEIGHT_MIN       2.0
#define GROOVE_HEIGHT_MAX       6.0

// - Constructions -

AcHorizontalSlider::AcHorizontalSlider(QWidget *parent)
  : Base(Qt::Horizontal, parent),
    grooveHeight_(GROOVE_HEIGHT_MIN),
    minimumGrooveHeight_(GROOVE_HEIGHT_MIN),
    maximumGrooveHeight_(GROOVE_HEIGHT_MAX)
{
  grooveHeightAni_ = new QPropertyAnimation(this, "grooveHeight", this);
  grooveHeightAni_->setDuration(400); // msecs
  updateStyleSheet();
}

// - Properties -

void
AcHorizontalSlider::setGrooveHeight(qreal value)
{
  if (!qFuzzyCompare(grooveHeight_, value)) {
    grooveHeight_ = value;
    updateStyleSheet();
  }
}

void
AcHorizontalSlider::updateStyleSheet()
{
  QString h = QString::number(grooveHeight_, 'f');
  if (isHovered())
    setStyleSheet(SS_BASE SS_HANDLE SS_GROOVE(+h+));
  else
    setStyleSheet(SS_BASE SS_NO_HANDLE SS_GROOVE(+h+));
}

// - Events -

void
AcHorizontalSlider::enterEvent(QEvent *event)
{
  if (isEnabled()) {
    if (grooveHeightAni_->state() != QAbstractAnimation::Stopped)
      grooveHeightAni_->stop();
    grooveHeightAni_->setStartValue(grooveHeight_);
    grooveHeightAni_->setEndValue(maximumGrooveHeight_);
    grooveHeightAni_->start();
  }
  Base::enterEvent(event);
}

void
AcHorizontalSlider::leaveEvent(QEvent *event)
{
  if (isEnabled()) {
    if (grooveHeightAni_->state() != QAbstractAnimation::Stopped)
      grooveHeightAni_->stop();
    grooveHeightAni_->setStartValue(grooveHeight_);
    grooveHeightAni_->setEndValue(minimumGrooveHeight_);
    QTimer::singleShot(800, grooveHeightAni_, SLOT(start()));
  }
  Base::leaveEvent(event);
}

void
AcHorizontalSlider::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::EnabledChange)
    updateStyleSheet();
  Base::changeEvent(event);
}

// EOF
