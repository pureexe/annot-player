// playerpanel.cc
// 7/15/2011

#include "playerpanel.h"
#include "inputcombobox.h"
#include "prefixcombobox.h"
#include "userlabel.h"
#include "defines.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "tr.h"
#include "module/qtext/slider.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

#define SS_TOOLBUTTON \
  SS_BEGIN(QToolButton) \
    SS_BORDERLESS \
  SS_END

#define SS_ALL \
  SS_TOOLBUTTON

PlayerPanel::PlayerPanel(QWidget *parent)
  : Base(parent),
    positionSlider_(0), volumeSlider_(0),
    playButton_(0), stopButton_(0), openButton_(0), nextFrameButton_(0),
    toggleFullScreenModeButton_(0), toggleMiniModeButton_(0), toggleEmbedModeButton_(0), toggleAnnotationButton_(0),
    userButton_(0), positionButton_(0), previousButton_(0), nextButton_(0),
    inputComboBox_(0), prefixComboBox_(0)
{
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(SS_ALL);
}

// - Sliders -
QSlider*
PlayerPanel::positionSlider()
{
  if (!positionSlider_) {
    positionSlider_ = new QtExt::Slider(Qt::Horizontal, this);
    positionSlider_->setStyleSheet(SS_SLIDER_POSITION);
    positionSlider_->setEnabled(false);
    positionSlider_->setMaximum(G_POSITION_MAX);
    positionSlider_->setToolTip(TR(T_POSITION));
    positionSlider_->setTracking(false);
    positionSlider_->setSingleStep(G_POSITION_STEP);
  } return positionSlider_;
}

QSlider*
PlayerPanel::volumeSlider()
{
  if (!volumeSlider_) {
    volumeSlider_ = new QtExt::Slider(Qt::Horizontal, this);
    volumeSlider_->setStyleSheet(SS_SLIDER_VOLUME);
    volumeSlider_->setMaximum(G_VOLUME_MAX);
    volumeSlider_->setToolTip(TR(T_VOLUME));
  } return volumeSlider_;
}

// - Buttons -

#define MAKE_BUTTON(_button, _styleid) \
  QToolButton* \
  PlayerPanel::_button() \
  { \
    if (!_button##_) { \
      _button##_ = new QtExt::ToolButton(this); \
      _button##_->setStyleSheet(SS_TOOLBUTTON_##_styleid); \
      _button##_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
      UiStyle::globalInstance()->setToolButtonStyle(_button##_); \
    } return _button##_; \
  }

  MAKE_BUTTON(openButton, OPEN)
  MAKE_BUTTON(playButton, PLAY)
  MAKE_BUTTON(stopButton, STOP)
  MAKE_BUTTON(nextFrameButton, NEXTFRAME)
  MAKE_BUTTON(toggleMiniModeButton, MINI)
  MAKE_BUTTON(toggleEmbedModeButton, EMBED)
  MAKE_BUTTON(toggleFullScreenModeButton, FULLSCREEN)
  MAKE_BUTTON(toggleAnnotationButton, ANNOT)
  MAKE_BUTTON(previousButton, PREVIOUS)
  MAKE_BUTTON(nextButton, NEXT)
  MAKE_BUTTON(userButton, USER)
  MAKE_BUTTON(positionButton, SEEK)
#undef MAKE_BUTTON

// - Line edits -

QComboBox*
PlayerPanel::inputComboBox()
{
  if (!inputComboBox_) {
    inputComboBox_ = new InputComboBox(this);
    inputComboBox_->setStyleSheet(SS_COMBOBOX);
    inputComboBox_->setToolTip(TR(T_TOOLTIP_INPUTLINE));
  } return inputComboBox_;
}

QComboBox*
PlayerPanel::prefixComboBox()
{
  if (!prefixComboBox_) {
    prefixComboBox_ = new PrefixComboBox(this);
    prefixComboBox_->setStyleSheet(SS_COMBOBOX);
    prefixComboBox_->setToolTip(TR(T_TOOLTIP_PREFIXLINE));
    prefixComboBox_->setMaximumWidth(G_PREFIXLINE_MAXWIDTH);
  } return prefixComboBox_;
}

// EOF
