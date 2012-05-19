#ifndef WBADDRESSEDIT_H
#define WBADDRESSEDIT_H

// gui/wbaddressedit.h
// 3/31/2012

#include "gui/wbcomboedit.h"
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class WbAddressEdit : public WbComboEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(WbAddressEdit)
  typedef WbAddressEdit Self;
  typedef WbComboEdit Base;

public:
  explicit WbAddressEdit(QWidget *parent = 0)
    : Base(parent), progress_(100) { init(); }
  explicit WbAddressEdit(const QStringList &items, QWidget *parent = 0)
    : Base(items, parent), progress_(100) { init(); }

signals:
  void openUrlWithAcPlayerRequested(const QString &url);
  void importUrlToAcPlayerRequested(const QString &url);
  void openUrlWithAcDownloaderRequested(const QString &url);
  void openUrlWithOperatingSystemRequested(const QString &url);

  // - Properties -
public slots:
  void setProgress(int value);

  // - Events -
protected:
  virtual void wheelEvent(QWheelEvent *) { } ///< \override disabled
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

protected slots:
  void updatePalette();

  void enterText()
  {
    QString t = currentText().trimmed();
    if (!t.isEmpty()) emit textEntered(t);
  }

  void openWithAcPlayer()
  { emit openUrlWithAcPlayerRequested(currentText().trimmed()); }

  void importToAcPlayer()
  { emit importUrlToAcPlayerRequested(currentText().trimmed()); }

  void openWithAcDownloader()
  { emit openUrlWithAcDownloaderRequested(currentText().trimmed()); }

  void openWithOperatingSystem()
  { emit openUrlWithOperatingSystemRequested(currentText().trimmed()); }

private:
  void init();
  void createActions();
  void createConnections();

protected:
  int progress_;
  QAction *openAddressWithAcPlayerAct_,
          *importAddressToAcPlayerAct_,
          *openAddressWithAcDownloaderAct_,
          *openWithOperatingSystemAct_;
};

#endif // WBADDRESSEDIT_H
