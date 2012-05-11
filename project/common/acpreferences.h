#ifndef ACPREFERENCES_H
#define ACPREFERENCES_H

// acpreferences.h
// 5/5/2012

#include "project/common/acmainwindow.h"
#include <QtCore/QList>

class AcPreferencesTab;
class AcSettings;
class AcTabView;
class AcPreferences : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(AcPreferences)
  typedef AcPreferences Self;
  typedef AcMainWindow Base;

  AcSettings *settings_;
  AcTabView *tabView_;
  ulong tabs_;

public:
  enum Tab {
    NoTab = 0x0,
    LocationTab = 1,
    AccountTab = 1,
    NetworkProxyTab = 1 << 1
  };

  explicit AcPreferences(ulong tabs, QWidget *parent = 0);
  explicit AcPreferences(QWidget *parent = 0);

  virtual QSize sizeHint() const;

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \override
protected slots:
  void save();
  void load();
  void updateSize();

private:
  void init();
  void createLayout();
  void addTab(AcPreferencesTab *tab);
};

#endif // ACPREFERENCES_H
