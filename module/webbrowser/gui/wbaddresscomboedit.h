#ifndef WBADDRESSCOMBOEDIT_H
#define WBADDRESSCOMBOEDIT_H

// wbaddresscomboedit.h
// 3/31/2012

#include "wbcomboedit.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class WbAddressComboEdit : public WbComboEdit
{
  Q_OBJECT
  typedef WbAddressComboEdit Self;
  typedef WbComboEdit Base;

public:
  explicit WbAddressComboEdit(QWidget *parent = 0)
    : Base(parent) { createActions(); }
  explicit WbAddressComboEdit(const QStringList &items, QWidget *parent = 0)
    : Base(items, parent) { createActions(); }

signals:
  void visitAddressRequested(const QString &url);

  // - Actions -
protected slots:
  void pasteAndGo();

  // - Events -
protected:
  //virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  static bool isClipboardEmpty();

private:
  void createActions();

protected:
  QAction *pasteAndGoAct;
};

#endif // WBADDRESSCOMBOEDIT_H
