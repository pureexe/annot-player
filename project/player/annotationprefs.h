#ifndef ANNOTATIONPREFS_H
#define ANNOTATIONPREFS_H

// annotationprefs.h
// 5/25/2012

#include "project/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QToolButton)

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
  QSpinBox *resolutionEdit_,
           *offsetEdit_;
  QFontComboBox *fontEdit_,
                *japaneseFontEdit_,
                *chineseFontEdit_;
  QToolButton *annotColorButton_,
              *subtitleColorButton_,
              *highlightColorButton_;

public:
  explicit AnnotationPreferencesTab(AnnotationSettings *settings, QWidget *parent = 0)
    : Base(parent), settings_(settings) { init(); }
  explicit AnnotationPreferencesTab(QWidget *parent = 0)
    : Base(parent), settings_(0) { init(); }

public slots:
  virtual bool save(); ///< \reimp
  virtual void load(); ///< \reimp
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

  void saveResolution();
  void loadResolution();
  void loadResolutionIfVisible() { if (isVisible()) loadResolution(); }
  void resetResolution();

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
