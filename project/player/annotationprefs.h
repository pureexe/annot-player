#ifndef ANNOTATIONPREFS_H
#define ANNOTATIONPREFS_H

// annotationprefs.h
// 5/25/2012

#include "project/common/acpreferencestab.h"

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QFontComboBox;
class QSpinBox;
class QToolButton;
QT_END_NAMESPACE

class AnnotationSettings;
class AnnotationPreferencesTab : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationPreferencesTab)
  typedef AnnotationPreferencesTab Self;
  typedef AcPreferencesTab Base;

  AnnotationSettings *settings_;

  //QComboBox *scaleEdit_,
  //          *rotationEdit_,
  QDoubleSpinBox *scaleEdit_,
                 *fullscreenScaleEdit_;
  QDoubleSpinBox *speedEdit_;
  QSpinBox *offsetEdit_,
           *opacityEdit_;
  QFontComboBox *fontEdit_,
                *japaneseFontEdit_,
                *chineseFontEdit_;
  QToolButton *annotColorButton_,
              *subtitleColorButton_,
              *highlightColorButton_;

public:
  explicit AnnotationPreferencesTab(AnnotationSettings *settings, QWidget *parent = nullptr)
    : Base(parent), settings_(settings) { init(); }
  explicit AnnotationPreferencesTab(QWidget *parent = nullptr)
    : Base(parent), settings_(nullptr) { init(); }

public slots:
  bool save() override;
  void load() override;
protected slots:
  void saveFont();
  void loadFont();
  void resetFont();

  void saveJapaneseFont();
  void loadJapaneseFont();
  void resetJapaneseFont();

  void saveChineseFont();
  void loadChineseFont();
  void resetChineseFont();

  void saveOpacity();
  void loadOpacity();
  void loadOpacityIfVisible() { if (isVisible()) loadOpacity(); }
  void resetOpacity();

  void saveScale();
  void loadScale();
  void loadScaleIfVisible() { if (isVisible()) loadScale(); }
  void resetScale();

  void saveFullscreenScale();
  void loadFullscreenScale();
  void loadFullscreenScaleIfVisible() { if (isVisible()) loadFullscreenScale(); }
  void resetFullscreenScale();

  void saveSpeed();
  void loadSpeed();
  void loadSpeedIfVisible() { if (isVisible()) loadSpeed(); }
  void resetSpeed();

  void saveOffset();
  void loadOffset();
  void loadOffsetIfVisible() { if (isVisible()) loadOffset(); }
  void resetOffset();

  void loadAnnotationColor();
  void resetAnnotationColor();
  void promptAnnotationColor();

  void loadSubtitleColor();
  void resetSubtitleColor();
  void promptSubtitleColor();

  void loadHighlightColor();
  void resetHighlightColor();
  void promptHighlightColor();
private:
  void init();
  void createLayout();
};

#endif // ANNOTATIONPREFS_H
