#ifndef WBCOMBOEDIT_H
#define WBCOMBOEDIT_H

// wbcomboedit.h
// 3/31/2012

#include "module/qtext/combobox.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

typedef QtExt::ComboBox WbComboEditBase;

class WbComboEdit : public WbComboEditBase
{
  Q_OBJECT
  typedef WbComboEdit Self;
  typedef WbComboEditBase Base;

public:
  explicit WbComboEdit(QWidget *parent = 0)
    : Base(parent) { init(); }
  explicit WbComboEdit(const QStringList &items, QWidget *parent = 0)
    : Base(parent), defaultItems_(items) { init(); }

  QStringList defaultItems() const { return defaultItems_; }

  // - Properties -
public slots:
  void reset();
  void setDefaultItems(const QStringList &l) { defaultItems_ = l; reset(); }

  // - Actions -
protected slots:
  void popup() { showPopup(); }

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  void init();
  void createActions();

protected:
  QAction *popupAct,
          *clearAct;
private:
  QStringList defaultItems_;
};

#endif // WBCOMBOEDIT_H
