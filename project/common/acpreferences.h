#ifndef ACPREFERENCES_H
#define ACPREFERENCES_H

// acpreferences.h
// 5/5/2012

#include "project/common/acmainwindow.h"
#include <QtCore/QList>

class AcSettings;
class AcPreferencesTab;
class AcPreferences : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(AcPreferences)
  typedef AcPreferences Self;
  typedef AcMainWindow Base;

  AcSettings *settings_;
  QList<AcPreferencesTab *> tabs_;

public:
  explicit AcPreferences(AcSettings *settings, QWidget *parent = 0)
    : Base(parent), settings_(settings) { init(); }
  explicit AcPreferences(QWidget *parent = 0)
    : Base(parent), settings_(0) { init(); }

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected slots:
  void save();
  void refresh();

private:
  void init();
  void createLayout();
  void addTab(AcPreferencesTab *tab);
};

#endif // ACPREFERENCES_H
