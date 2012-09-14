#ifndef _QTEXT_WEBPAGE_H
#define _QTEXT_WEBPAGE_H

// qtext/webpage.h
// 4/9/2012

#include <QtNetwork/QNetworkRequest>
#include <QtWebKit/QWebPage>

namespace QtExt {

class WebPage: public QWebPage
{
  Q_OBJECT
  Q_DISABLE_COPY(WebPage)
  typedef WebPage Self;
  typedef QWebPage Base;

  QString hoveredLink_;
  bool hoveredLinkLocked_;

public:
  explicit WebPage(QWidget *parent = nullptr);

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

} // namespace QtExt

#endif // _QTEXT_WEBPAGE_H
