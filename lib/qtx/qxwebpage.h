#ifndef QXWEBPAGE_H
#define QXWEBPAGE_H

// qxwebpage.h
// 4/9/2012

#include "qtx/qtxglobal.h"
#include <QtNetwork/QNetworkRequest>
#include <QtWebKit/QWebPage>

QTX_BEGIN_NAMESPACE

class QxWebPage: public QWebPage
{
  Q_OBJECT
  Q_DISABLE_COPY(QxWebPage)
  typedef QxWebPage Self;
  typedef QWebPage Base;

  QString hoveredLink_;
  bool hoveredLinkLocked_;

public:
  explicit QxWebPage(QWidget *parent = nullptr);

  QString hoveredLink() const { return hoveredLink_; }

  bool event(QEvent *e) override;

public slots:
  void scrollTop();
  void scrollBottom();
  void scrollLeft();
  void scrollRight();

signals:
  void openLinkRequested(QString url);

private slots:
  void setHoveredLink(const QString &url)
  { hoveredLink_ = url; }

  // - Extensions -
public:
  bool supportsExtension(Extension extension) const override;
  bool extension(Extension extension, const ExtensionOption *option = nullptr, ExtensionReturn *output = nullptr) override;

protected:
  bool errorPageExtension(const ErrorPageExtensionOption *option = nullptr, ErrorPageExtensionReturn *output = nullptr);
  //QObject *createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues) override;

  QString userAgentForUrl(const QUrl &url) const override;
};

QTX_END_NAMESPACE

#endif // QXWEBPAGE_H
