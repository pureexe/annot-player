#ifndef WBCOMBOEDIT_H
#define WBCOMBOEDIT_H

// gui/wbcomboedit.h
// 3/31/2012
#include "module/qtext/combobox.h"
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

typedef QtExt::ComboBox WbComboEditBase;

class WbComboEdit : public WbComboEditBase
{
  Q_OBJECT
  Q_DISABLE_COPY(WbComboEdit)
  typedef WbComboEdit Self;
  typedef WbComboEditBase Base;

public:
  explicit WbComboEdit(QWidget *parent = 0)
    : Base(parent) { init(); }
  explicit WbComboEdit(const QStringList &items, QWidget *parent = 0)
    : Base(parent), defaultItems_(items) { init(); }

  QStringList defaultItems() const { return defaultItems_; }

signals:
  void textEntered(const QString &url);
  void styleSheetChanged();

  // - Properties -
public slots:
  void setIcon(const QString &url);
  void clearIcon() { setIcon(QString()); }

  void reset();
  virtual void submitText();
  void pasteAndGo();
  void setDefaultItems(const QStringList &l, const QStringList &icons = QStringList())
  { defaultItems_ = l; defaultIcons_ = icons; reset(); }

  // - Events -
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp

  static bool isClipboardEmpty();

private:
  void init();
  void createActions();

protected:
  QAction *popupAct,
          *clearAct,
          *pasteAndGoAct,
          *submitAct;
private:
  QStringList defaultItems_,
               defaultIcons_;
};

#endif // WBCOMBOEDIT_H
