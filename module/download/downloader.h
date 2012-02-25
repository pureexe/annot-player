#ifndef DOWNLOADER_H
#define DOWNLOADER_H

// downloader.h
// 2/3/2012

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class Downloader : public QObject
{
  Q_OBJECT
  typedef Downloader Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  int state_;
  QString path_;
  bool stopped_;

public:
  enum State { Error = -1, OK = 0, Downloading = 1 };

public:
 explicit Downloader(const QString &path, QObject *parent = 0)
    : Base(parent), state_(OK), path_(path), stopped_(false) { init(); }
 explicit Downloader(QObject *parent = 0)
    : Base(parent), state_(OK), stopped_(false) { init(); }

  int state() const { return state_; }

  QNetworkAccessManager *networkAccessManager() { return nam_; }

private:
  void init();

signals:
  void progress(qint64 bytesReceived, qint64 bytesTotal);
  void finished();
  void error(const QString &message);

public slots:
  void setPath(const QString &path) { path_ = path; }
  void get(const QUrl &url,
           const QString &header = QString(), bool async = true, int retries = 3);
  void post(const QUrl &url, const QByteArray &data = QByteArray(),
            const QString &header = QString(), bool async = true, int retries = 3);

  QNetworkAccessManager *networkAccessManager() const { return nam_; }

  void stop() { stopped_ = true; }

protected slots:
  void save(QNetworkReply *reply);
  void redirect(QNetworkReply *reply);
  static bool save(const QByteArray &data, const QString &path);

  static QHash<QString, QString> parseHttpHeader(const QString &header);

protected:
  static QByteArray encodeUrlParameters(const QString &params);
};

#endif // DOWNLOADER_H
