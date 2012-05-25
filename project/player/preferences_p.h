#ifndef PREFERENCES_P_H
#define PREFERENCES_P_H

// preferences_p.h
// 5/25/2012

#include "project/common/acpreferencestab.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)
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
  QComboBox     *offsetEdit_;
  QFontComboBox *fontEdit_;

  QString offsetEditStyleSheet_;

public:
  explicit AnnotationPreferencesTab(AnnotationSettings *settings, QWidget *parent = 0);

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
  void createLayout();
};

#endif // PREFERENCES_P_H
