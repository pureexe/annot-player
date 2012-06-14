#ifndef ACCOMBOEDIT_H
#define ACCOMBOEDIT_H

// accomboedit.h
// 7/16/2011

#include "module/qtext/combobox.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)

typedef QtExt::ComboBox ComboEditBase;
class AcComboEdit : public ComboEditBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcComboEdit)
  typedef AcComboEdit Self;
  typedef ComboEditBase Base;

public:
  enum ContextMenuFlag {
    NoAction = 0,
    PopupAction = 1,            // enabled by default
    ClearAction = 1 << 1,       // enabled by default
    PasteAndGoAction = 1 << 2,  // disabled by default
    CustomAction = 1 << 3
  };

  explicit AcComboEdit(QWidget *parent = 0)
    : Base(parent) { init(); }
  explicit AcComboEdit(const QStringList &items, QWidget *parent = 0)
    : Base(parent), defaultItems(items) { init(); }

  uint contextMenuFlags() const { return contextMenuFlags_; }
  void setContextMenuFlags(uint flags) { contextMenuFlags_ = flags; }

  // - Properties -
public slots:
  void reset();
  void setDefaultItems(const QStringList &l) { defaultItems = l; reset(); }

  // - Actions -
protected slots:
  void pasteAndGo();

  // - Events -
protected:
  //virtual void keyPressEvent(QKeyEvent *event); ///< \reimp
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp

protected:
  static bool isClipboardEmpty();

private:
  void init();
  void createActions();

protected:
  uint contextMenuFlags_;
  QStringList defaultItems;
  QAction *popupAct,
          *clearAct,
          *pasteAndGoAct;
};

#endif // ACCOMBOEDIT_H
