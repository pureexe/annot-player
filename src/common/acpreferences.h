#ifndef ACPREFERENCES_H
#define ACPREFERENCES_H

// acpreferences.h
// 5/5/2012

#include "src/common/acmainwindow.h"

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

  explicit AcPreferences(ulong tabs, QWidget *parent = nullptr, bool finalize = true);
  explicit AcPreferences(QWidget *parent = nullptr, bool finalize = true);

  void finalize();
  void setTabs(ulong mask) { tabs_ = mask; }

  QSize sizeHint() const override;

  // - Events -
public slots:
  void setVisible(bool visible) override;
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
