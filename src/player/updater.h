#ifndef UPDATER_H
#define UPDATER_H

// updater.h
// 5/27/2013

#include <QtCore/QHash>
#include <QtCore/QFile>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkConfigurationManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class Updater : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Updater)
  typedef Updater Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  QNetworkConfigurationManager *ncm_;

public:
  explicit Updater(QObject *parent = nullptr);

signals:
  void versionReceived(long timestamp);

public:
  void queryVersion();
  bool isOnline() const;

protected slots:
  void parseReply(QNetworkReply *reply);
  void parseData(const QByteArray &data);
};

#endif // ANNOTATIONCODEC_H
