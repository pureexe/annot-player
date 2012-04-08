#ifndef PLAYERPANEL_H
#define PLAYERPANEL_H

// playerpanel.h
// 7/15/2011

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QSlider;
class QToolButton;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class PositionSlider;

class PlayerPanel: public QWidget
{
  Q_OBJECT
  typedef PlayerPanel Self;
  typedef QWidget Base;

public:
  explicit PlayerPanel(QWidget *parent = 0);

#define ADD_WIDGET(_type, _name) \
  public: _type *_name(); \
  private: _type *_name##_;

  ADD_WIDGET(PositionSlider, positionSlider)
  ADD_WIDGET(QSlider, volumeSlider)
  ADD_WIDGET(QToolButton, openButton)
  ADD_WIDGET(QToolButton, playButton)
  ADD_WIDGET(QToolButton, stopButton)
  ADD_WIDGET(QToolButton, nextFrameButton)
  ADD_WIDGET(QToolButton, fastForwardButton)
  ADD_WIDGET(QToolButton, fastFastForwardButton)
  //ADD_WIDGET(QToolButton, rewindButton)
  ADD_WIDGET(QToolButton, toggleFullScreenModeButton)
  ADD_WIDGET(QToolButton, toggleMiniModeButton)
  ADD_WIDGET(QToolButton, toggleEmbedModeButton)
  //ADD_WIDGET(QToolButton, toggleAnnotationButton)
  ADD_WIDGET(QToolButton, userButton)
  ADD_WIDGET(QToolButton, positionButton)
  ADD_WIDGET(QToolButton, previousButton)
  ADD_WIDGET(QToolButton, nextButton)
  ADD_WIDGET(QToolButton, menuButton)
  //ADD_WIDGET(QLineEdit, lineEdit)
  //ADD_WIDGET(QLineEdit, prefixLineEdit)
  ADD_WIDGET(QToolButton, inputCountButton)
  ADD_WIDGET(QComboBox, inputComboBox)
  ADD_WIDGET(QComboBox, prefixComboBox)
#undef ADD_WIDGET
};

#endif // PLAYERPANEL_H
