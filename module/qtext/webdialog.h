#ifndef _QTEXT_WEBDIALOG_H
#define _QTEXT_WEBDIALOG_H

// qtext/webdialog.h
// 10/9/2011

#include <QWebView>
#include <QNetworkRequest>

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QAction)

namespace QtExt {

class WebDialog: public QWebView
{
  Q_OBJECT
  typedef WebDialog Self;
  typedef QWebView Base;

public:
  explicit WebDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
private:
  void createActions();

  //-  Log -
signals:
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);

public:
  QString hoveredLink() const;

protected slots:
  void download(const QNetworkRequest &req);
  void zoomIn();
  void zoomOut();
  void zoomReset();

protected:
  virtual void contextMenuEvent(QContextMenuEvent *e); ///< \override
  virtual void wheelEvent(QWheelEvent *e); ///< \override

  QMenu *createContextMenu();
protected:
  QAction *zoomInAct,
          *zoomOutAct,
          *zoomResetAct;
};

} // namespace QtExt

#endif // _QTEXT_WEBDIALOG_H
