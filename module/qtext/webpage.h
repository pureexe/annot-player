#ifndef _QTEXT_WEBPAGE_H
#define _QTEXT_WEBPAGE_H

// qtext/webpage.h
// 4/9/2012

#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkRequest>

namespace QtExt {

class WebPage: public QWebPage
{
  Q_OBJECT
  typedef WebPage Self;
  typedef QWebPage Base;

  QString hoveredLink_;
  bool hoveredLinkLocked_;

public:
  explicit WebPage(QWidget *parent = 0);

  QString hoveredLink() const { return hoveredLink_; }

  virtual bool event(QEvent *e); ///< \override

signals:
  void openLinkRequested(QString url);

private slots:
  void setHoveredLink(const QString &url)
  { hoveredLink_ = url; }

  // - Extensions -
public:
  virtual bool supportsExtension(Extension extension) const; ///< \override
  virtual bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0); ///< \override

protected:
  bool errorPageExtension(const ErrorPageExtensionOption *option = 0, ErrorPageExtensionReturn *output = 0);
  //virtual QObject *createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues); ///< \override
};

} // namespace QtExt

#endif // _QTEXT_WEBPAGE_H
