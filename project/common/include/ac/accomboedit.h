#ifndef _AC_ACCOMBOEDIT_H
#define _AC_ACCOMBOEDIT_H

// ac/accomboedit.h
// 7/16/2011

#include "module/qtext/combobox.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

typedef QtExt::ComboBox ComboEditBase;
class AcComboEdit : public ComboEditBase
{
  Q_OBJECT
  typedef AcComboEdit Self;
  typedef ComboEditBase Base;

public:
  explicit AcComboEdit(QWidget *parent = 0)
    : Base(parent) { init(); }
  explicit AcComboEdit(const QStringList &items, QWidget *parent = 0)
    : Base(parent), defaultItems(items) { init(); }

  // - Properties -
public slots:
  void reset();
  void setDefaultItems(const QStringList &l) { defaultItems = l; reset(); }

  // - Actions -
protected slots:
  void popup() { showPopup(); }

  // - Events -
protected:
  //virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  void init();
  void createActions();

protected:
  QStringList defaultItems;
  QMenu *contextMenu;
  QAction *popupAct,
          *clearAct;
};

#endif // _AC_ACCOMBOEDIT_H
