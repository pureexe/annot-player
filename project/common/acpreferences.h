#ifndef ACPREFERENCES_H
#define ACPREFERENCES_H

// acpreferences.h
// 5/5/2012

#include "project/common/acmainwindow.h"

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
    AllTabs = 0x0,
    LocationTab = 1,
    AccountTab = 1,
    NetworkProxyTab = 1 << 1
  };

  explicit AcPreferences(ulong tabs, QWidget *parent = 0, bool finalize = true);
  explicit AcPreferences(QWidget *parent = 0, bool finalize = true);

  void finalize();
  void setTabs(ulong mask) { tabs_ = mask; }

  virtual QSize sizeHint() const; ///< \reimp

  // - Events -
public slots:
  virtual void setVisible(bool visible); ///< \reimp
protected slots:
  void save();
  void load();
  void clear();
  void updateSize();

protected:
  void addTab(AcPreferencesTab *tab);
private:
  void init();
  void createLayout();
};

#endif // ACPREFERENCES_H
