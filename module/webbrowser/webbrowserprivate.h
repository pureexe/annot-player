#ifndef WEBBROWSERPRIVATE_H
#define WEBBROWSERPRIVATE_H

// webbrowserprivate.h
// 1/27/2012

#include "module/qtext/network.h"
#include <QtGui>
#include <QtNetwork>

// - Slots -

namespace slot_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabText(QTabWidget *w, int index)
      : Base(w), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void setTabText(const QString &text)
    {
      if (w_->count() > index_)
        w_->setTabText(index_, text);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace slot_

typedef QtExt::NetworkCookieJarWithDomainAlias WBNetworkCookieJarBase;
class WBNetworkCookieJar : public WBNetworkCookieJarBase
{
  Q_OBJECT
  typedef WBNetworkCookieJar Self;
  typedef WBNetworkCookieJarBase Base;

public:
  explicit WBNetworkCookieJar(QObject *parent = 0)
    : Base(".nicovideo.jp", ".galstars.net", parent)
  { }
};

class WBNetworkAccessManager : public QNetworkAccessManager
{
  Q_OBJECT
  typedef WBNetworkAccessManager Self;
  typedef QNetworkAccessManager Base;

public:
  explicit WBNetworkAccessManager(QObject *parent = 0)
    : Base(parent) { }

protected:
  ///  \override
  virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &req,
                                       QIODevice *outgoingData = 0);

  static QUrl transformNicoUrl(const QUrl &url);
  static QUrl transformNicoHostUrl(const QUrl &url);
  //static QNetworkReply *transformRedirectedReply(QNetworkReply *reply);
};

#endif // WEBBROWSERPRIVATE_H
