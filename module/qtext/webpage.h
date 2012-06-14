#ifndef _QTEXT_WEBPAGE_H
#define _QTEXT_WEBPAGE_H

// qtext/webpage.h
// 4/9/2012

#include <QtWebKit/QWebPage>
#include <QtNetwork/QNetworkRequest>

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
  explicit WebPage(QWidget *parent = 0);

  QString hoveredLink() const { return hoveredLink_; }

  virtual bool event(QEvent *e); ///< \reimp

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
  virtual bool supportsExtension(Extension extension) const; ///< \reimp
  virtual bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0); ///< \reimp

protected:
  bool errorPageExtension(const ErrorPageExtensionOption *option = 0, ErrorPageExtensionReturn *output = 0);
  //virtual QObject *createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues); ///< \reimp

  virtual QString userAgentForUrl(const QUrl &url) const; ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_WEBPAGE_H
