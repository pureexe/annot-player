#ifndef WBADDRESSEDIT_H
#define WBADDRESSEDIT_H

// wbaddressedit.h
// 3/31/2012

#include "wbcomboedit.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class WbAddressEdit : public WbComboEdit
{
  Q_OBJECT
  typedef WbAddressEdit Self;
  typedef WbComboEdit Base;

public:
  explicit WbAddressEdit(QWidget *parent = 0)
    : Base(parent) { createActions(); }
  explicit WbAddressEdit(const QStringList &items, QWidget *parent = 0)
    : Base(items, parent) { createActions(); }

signals:
  void textEntered(const QString &url);
  void openUrlWithAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);

  // - Actions -
protected slots:
  void pasteAndGo();

  // - Events -
protected:
  //virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  static bool isClipboardEmpty();

private slots:
  void openAddressWithAcPlayer()
  { emit openUrlWithAcPlayerRequested(currentText().trimmed()); }

  void openAddressWithAcDownloader()
  { emit openUrlWithAcDownloaderRequested(currentText().trimmed()); }

private:
  void createActions();

protected:
  QAction *pasteAndGoAct,
          *openAddressWithAcPlayerAct_,
          *openAddressWithAcDownloaderAct_;
};

#endif // WBADDRESSEDIT_H
