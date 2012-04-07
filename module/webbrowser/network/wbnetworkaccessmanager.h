#ifndef WBNETWORKACCESSMANAGER_H
#define WBNETWORKACCESSMANAGER_H

// wbnetwork.h
// 1/27/2012

#include "module/qtext/network.h"

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

#endif // WBNETWORKACCESSMANAGER_H
