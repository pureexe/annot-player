#ifndef WEBBROWSERPRIVATE_H
#define WEBBROWSERPRIVATE_H

// webbrowserprivate.h
// 1/27/2012

#include "module/qtext/network.h"
#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>

//#define NICO_PROXY_DOMAIN       ".galstars.net"
#define NICO_PROXY_DOMAIN       "sakuradite.com"

// - Slots -

namespace slot_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabText(QTabWidget *w, int index, QWidget *parent = 0)
      : Base(parent), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void setTabText(const QString &text)
    {
      if (index_ < w_->count())
        w_->setTabText(index_, text);
    }
  };

} // namespace slot_

typedef QtExt::NetworkCookieJarWithDomainAlias WbNetworkCookieJarBase;
class WbNetworkCookieJar : public WbNetworkCookieJarBase
{
  Q_OBJECT
  typedef WbNetworkCookieJar Self;
  typedef WbNetworkCookieJarBase Base;

public:
  explicit WbNetworkCookieJar(QObject *parent = 0)
    : Base(".nicovideo.jp", NICO_PROXY_DOMAIN, parent)
  { }
};

class WbNetworkAccessManager : public QNetworkAccessManager
{
  Q_OBJECT
  typedef WbNetworkAccessManager Self;
  typedef QNetworkAccessManager Base;

public:
  explicit WbNetworkAccessManager(QObject *parent = 0)
    : Base(parent) { }

protected:
  ///  \override
  virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &req,
                                       QIODevice *outgoingData = 0);

  static QUrl transformNicoUrl(const QUrl &url);
  static QUrl transformAb2Url(const QUrl &url);
  static QUrl transformSyangrilaUrl(const QUrl &url);
  static QUrl transformEroUrl(const QUrl &url);
};

class WbWebView : public QWebView
{
  Q_OBJECT
  typedef WbWebView Self;
  typedef QWebView Base;
public:
  explicit WbWebView(QWidget *parent = 0) : Base(parent) { }

signals:
  void windowCreated(QWebView *w);

protected:
  virtual Base *createWindow(QWebPage::WebWindowType type) ///< \override
  {
    if (type != QWebPage::WebBrowserWindow)
      return Base::createWindow(type);
    Self *ret = new Self(parentWidget());
    emit windowCreated(ret);
    return ret;
  }
};

#endif // WEBBROWSERPRIVATE_H
