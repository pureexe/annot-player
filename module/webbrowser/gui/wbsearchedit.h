#ifndef WBSEARCHEDIT_H
#define WBSEARCHEDIT_H

// wbsearchedit.h
// 3/31/2012

#include "wbcomboedit.h"

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class WbSearchEdit : public WbComboEdit
{
  Q_OBJECT
  typedef WbSearchEdit Self;
  typedef WbComboEdit Base;

public:
  explicit WbSearchEdit(QWidget *parent = 0)
    : Base(parent) { createActions(); }
  explicit WbSearchEdit(const QStringList &items, QWidget *parent = 0)
    : Base(items, parent) { createActions(); }

signals:
  void textEntered(const QString &url);

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

#endif // WBSEARCHEDIT_H
