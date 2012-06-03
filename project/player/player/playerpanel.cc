// playerpanel.cc
// 7/15/2011

#include "playerpanel.h"
#include "inputcombobox.h"
#include "prefixcombobox.h"
#include "positionslider.h"
#include "userlabel.h"
#include "global.h"
#include "stylesheet.h"
#include "tr.h"
#include "project/common/achorizontalslider.h"
#include "project/common/acui.h"
#include "project/common/aciconbutton.h"
#include <QtGui>

//enum { VOLUME_SLIDE_MAX_WIDTH = 100 };

// - Construction -

PlayerPanel::PlayerPanel(QWidget *parent)
  : Base(parent),
    positionSlider_(0), volumeSlider_(0),
    openButton_(0), playButton_(0), stopButton_(0), nextFrameButton_(0), fastForwardButton_(0), fastFastForwardButton_(0),
    toggleFullScreenModeButton_(0), toggleMiniModeButton_(0), toggleEmbedModeButton_(0), toggleTraceWindowButton_(0),
    userButton_(0), positionButton_(0), progressButton_(0), previousButton_(0), nextButton_(0), menuButton_(0),
    inputCountButton_(0), inputComboBox_(0), prefixComboBox_(0)
{
  setContentsMargins(0, 0, 0, 0);
  setStyleSheet(
    SS_BEGIN(QToolButton) \
      SS_BORDERLESS \
    SS_END
  );
}

// - Sliders -
PositionSlider*
PlayerPanel::positionSlider()
{
  if (!positionSlider_) {
    positionSlider_ = new PositionSlider(this);
    //positionSlider_->setStyleSheet(SS_SLIDER_POSITION);
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
    volumeSlider_ = new AcHorizontalSlider(this);
    volumeSlider_->setMaximum(G_VOLUME_MAX);
    volumeSlider_->setToolTip(TR(T_VOLUME));
    //volumeSlider_->setMaximumWidth(VOLUME_SLIDE_MAX_WIDTH);
  } return volumeSlider_;
}

// - Buttons -

#define MAKE_BUTTON(_button, _styleid) \
  QToolButton* \
  PlayerPanel::_button() \
  { \
    if (!_button##_) { \
      _button##_ = new AcIconButton(this); \
      _button##_->setStyleSheet(SS_TOOLBUTTON_##_styleid); \
      _button##_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
      AcUi::globalInstance()->setToolButtonStyle(_button##_); \
    } return _button##_; \
  }

  MAKE_BUTTON(openButton, OPEN)
  MAKE_BUTTON(playButton, PLAY)
  MAKE_BUTTON(stopButton, STOP)
  MAKE_BUTTON(nextFrameButton, NEXTFRAME)
  MAKE_BUTTON(fastForwardButton, FASTFORWARD)
  MAKE_BUTTON(fastFastForwardButton, FASTFASTFORWARD)
  //MAKE_BUTTON(rewindButton, REWIND)
  MAKE_BUTTON(toggleMiniModeButton, MINI)
  MAKE_BUTTON(toggleEmbedModeButton, EMBED)
  MAKE_BUTTON(toggleTraceWindowButton, TRACE)
  MAKE_BUTTON(toggleFullScreenModeButton, FULLSCREEN)
  //MAKE_BUTTON(toggleAnnotationButton, ANNOT)
  MAKE_BUTTON(previousButton, PREVIOUS)
  MAKE_BUTTON(nextButton, NEXT)
  MAKE_BUTTON(userButton, USER)
  MAKE_BUTTON(positionButton, SEEK)
  MAKE_BUTTON(progressButton, PROGRESS)
  MAKE_BUTTON(menuButton, MENU)
#undef MAKE_BUTTON

QToolButton*
PlayerPanel::inputCountButton()
{
  if (!inputCountButton_)
    inputCountButton_ = AcUi::globalInstance()->makeToolButton(
          0, "-", tr("Character count"));
  return inputCountButton_;
}

// - Line edits -

QComboBox*
PlayerPanel::inputComboBox()
{
  if (!inputComboBox_) {
    inputComboBox_ = new InputComboBox(this);
    inputComboBox_->setToolTip(TR(T_TOOLTIP_INPUTLINE));
  } return inputComboBox_;
}

QComboBox*
PlayerPanel::prefixComboBox()
{
  if (!prefixComboBox_) {
    prefixComboBox_ = new PrefixComboBox(this);
    prefixComboBox_->setToolTip(TR(T_TOOLTIP_PREFIXLINE));
    prefixComboBox_->setMaximumWidth(G_PREFIXLINE_MAXWIDTH);
  } return prefixComboBox_;
}

// EOF
