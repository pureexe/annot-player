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
#include "module/qtext/toolbutton.h"
#include <QtGui>

//#define ACSS_TOOLBUTTON_TEXT_(_weight, _style, _dec, _normal, _hover, _pressed, _checked, _checked_hover, _disabled)
#define SS_TOOLBUTTON_USER      ACSS_TOOLBUTTON_TEXT_(bold, normal, none, blue, red, orange, orange, red, gray)
#define SS_TOOLBUTTON_INTERNET  ACSS_TOOLBUTTON_TEXT_(normal, italic, none, blue, red, red, cyan, red, gray)
#define SS_TOOLBUTTON_SEEK      ACSS_TOOLBUTTON_TEXT_(normal, normal, none, cyan, red, orange, orange, red, gray)
#define SS_TOOLBUTTON_PROGRESS  ACSS_TOOLBUTTON_TEXT_(normal, italic, none, cyan, red, orange, orange, red, gray)

//enum { VOLUME_SLIDE_MAX_WIDTH = 100 };

// - Construction -

PlayerPanel::PlayerPanel(QWidget *parent)
  : Base(parent),
    positionSlider_(nullptr), volumeSlider_(nullptr),
    openButton_(nullptr), playButton_(nullptr), stopButton_(nullptr), nextFrameButton_(nullptr), fastForwardButton_(nullptr),
    toggleFullScreenModeButton_(nullptr), toggleMiniModeButton_(nullptr), toggleEmbedModeButton_(nullptr), toggleTraceWindowButton_(nullptr),
    userButton_(nullptr), networkButton_(nullptr), positionButton_(nullptr), progressButton_(nullptr), previousButton_(nullptr), nextButton_(nullptr),
    menuButton_(nullptr), libraryButton_(nullptr),
    inputCountButton_(nullptr), inputComboBox_(nullptr), prefixComboBox_(nullptr)
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

#define MAKE_ICON_BUTTON(_button, _styleid) \
  QToolButton* \
  PlayerPanel::_button() \
  { \
    if (!_button##_) { \
      _button##_ = new AcIconButton(this); \
      _button##_->setStyleSheet(SS_TOOLBUTTON_##_styleid); \
      _button##_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
      _button##_->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(Qt::red)); \
    } return _button##_; \
  }
  MAKE_ICON_BUTTON(playButton, PLAY)
#undef MAKE_ICON_BUTTON

#define MAKE_ICON_BUTTON(_button, _styleid) \
  QToolButton* \
  PlayerPanel::_button() \
  { \
    if (!_button##_) { \
      _button##_ = new AcIconButton(this); \
      _button##_->setStyleSheet(SS_TOOLBUTTON_##_styleid); \
      _button##_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
      _button##_->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(Qt::yellow)); \
    } return _button##_; \
  }

  MAKE_ICON_BUTTON(openButton, OPEN)
  //MAKE_ICON_BUTTON(playButton, PLAY)
  MAKE_ICON_BUTTON(stopButton, STOP)
  MAKE_ICON_BUTTON(nextFrameButton, NEXTFRAME)
  MAKE_ICON_BUTTON(fastForwardButton, FASTFORWARD)
  //MAKE_ICON_BUTTON(fastFastForwardButton, FASTFASTFORWARD)
  //MAKE_ICON_BUTTON(rewindButton, REWIND)
  MAKE_ICON_BUTTON(toggleMiniModeButton, MINI)
  MAKE_ICON_BUTTON(toggleEmbedModeButton, EMBED)
  MAKE_ICON_BUTTON(toggleTraceWindowButton, TRACE)
  MAKE_ICON_BUTTON(toggleFullScreenModeButton, FULLSCREEN)
  //MAKE_ICON_BUTTON(toggleAnnotationButton, ANNOT)
  MAKE_ICON_BUTTON(previousButton, PREVIOUS)
  MAKE_ICON_BUTTON(nextButton, NEXT)
  MAKE_ICON_BUTTON(menuButton, MENU)
  MAKE_ICON_BUTTON(libraryButton, LIBRARY)
#undef MAKE_ICON_BUTTON

#define MAKE_TEXT_BUTTON(_button, _styleid) \
  QToolButton* \
  PlayerPanel::_button() \
  { \
    if (!_button##_) { \
      _button##_ = new QtExt::ToolButton(this); \
      _button##_->setStyleSheet(SS_TOOLBUTTON_##_styleid); \
      _button##_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
    } return _button##_; \
  }
  MAKE_TEXT_BUTTON(networkButton, INTERNET)
  MAKE_TEXT_BUTTON(userButton, USER)
  MAKE_TEXT_BUTTON(positionButton, SEEK)
  MAKE_TEXT_BUTTON(progressButton, PROGRESS)

#undef MAKE_TEXT_BUTTON

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
