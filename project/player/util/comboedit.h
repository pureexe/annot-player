#ifndef COMBOEDIT_H
#define COMBOEDIT_H

// comboedit.h
// 7/16/2011

#include "module/qtext/combobox.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

typedef QtExt::ComboBox ComboEditBase;
class ComboEdit : public ComboEditBase
{
  Q_OBJECT
  typedef ComboEdit Self;
  typedef ComboEditBase Base;

public:
  explicit ComboEdit(QWidget *parent = 0);
  explicit ComboEdit(const QStringList &items, QWidget *parent = 0);

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

#endif // COMBOEDIT_H
