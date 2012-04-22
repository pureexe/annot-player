#ifndef WBADDRESSEDIT_H
#define WBADDRESSEDIT_H

// wbaddressedit.h
// 3/31/2012

#include "wbcomboedit.h"

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
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);

  // - Actions -
protected slots:
  void pasteAndGo();

  // - Events -
protected:
  virtual void wheelEvent(QWheelEvent *) { } ///< \override disabled
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  static bool isClipboardEmpty();

protected slots:
  void openWithAcPlayer()
  { emit openUrlWithAcPlayerRequested(currentText().trimmed()); }

  void importToAcPlayer()
  { emit importUrlToAcPlayerRequested(currentText().trimmed()); }

  void openWithAcDownloader()
  { emit openUrlWithAcDownloaderRequested(currentText().trimmed()); }

private:
  void createActions();

protected:
  QAction *pasteAndGoAct,
          *openAddressWithAcPlayerAct_,
          *importAddressToAcPlayerAct_,
          *openAddressWithAcDownloaderAct_;
};

#endif // WBADDRESSEDIT_H
