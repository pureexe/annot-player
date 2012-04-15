#ifndef WBNETWORKACCESSMANAGER_H
#define WBNETWORKACCESSMANAGER_H

// wbnetwork.h
// 1/27/2012

#include "module/qtext/network.h"
#include <QStringList>
#include <QUrl>

class WbNetworkAccessManager : public QNetworkAccessManager
{
  Q_OBJECT
  typedef WbNetworkAccessManager Self;
  typedef QNetworkAccessManager Base;

  QList<QUrl> blockedUrls_;

public:
  explicit WbNetworkAccessManager(QObject *parent = 0)
    : Base(parent) { }

public slots:
  void setBlockedUrls(const QList<QUrl> &urls) { blockedUrls_ = urls; }

protected:
  ///  \override
  virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &req,
                                       QIODevice *outgoingData = 0);
  bool isBlockedUrl(const QUrl &url) const;
  static QUrl transformNicoUrl(const QUrl &url);
  static QUrl transformAb2Url(const QUrl &url);
  static QUrl transformSyangrilaUrl(const QUrl &url);
  static QUrl transformEroUrl(const QUrl &url);
};

#endif // WBNETWORKACCESSMANAGER_H
