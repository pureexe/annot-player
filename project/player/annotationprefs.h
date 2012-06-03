#ifndef ANNOTATIONPREFS_H
#define ANNOTATIONPREFS_H

// annotationprefs.h
// 5/25/2012

#include "project/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)

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
  QSpinBox *offsetEdit_;
  QFontComboBox *fontEdit_;

public:
  explicit AnnotationPreferencesTab(AnnotationSettings *settings, QWidget *parent = 0)
    : Base(parent), settings_(settings) { init(); }
  explicit AnnotationPreferencesTab(QWidget *parent = 0)
    : Base(parent), settings_(0) { init(); }

public slots:
  virtual bool save(); ///< \override
  virtual void load(); ///< \override
protected slots:
  void saveFontFamily();
  void loadFontFamily();
  void resetFontFamily();

  void saveOffset();
  void loadOffset();
  void loadOffsetIfVisible() { if (isVisible()) loadOffset(); }
  void resetOffset();
private:
  void init();
  void createLayout();
};

#endif // ANNOTATIONPREFS_H
