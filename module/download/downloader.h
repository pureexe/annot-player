#ifndef DOWNLOADER
#define DOWNLOADER

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

  QNetworkAccessManager *qnam_;
  int state_;
  QString path_;
  bool zipped_;

public:
  enum State { Error = -1, OK = 0, Downloading = 1 };

public:
  explicit Downloader(const QString &path, bool zipped = false, QObject *parent = 0,
                      QNetworkAccessManager *qnam = 0);

  int state() const { return state_; }

public slots:
  void get(const QUrl &url, const QString &header = QString(), bool async = true);
  void post(const QUrl &url, const QByteArray &data = QByteArray(), const QString &header = QString(), bool async = true);

  static QByteArray encodeUrlParameters(const QString &params);

protected slots:
  void save(QNetworkReply *reply);
  static bool save(const QByteArray &data, const QString &path);

  static QHash<QString, QString> parseHttpHeader(const QString &header);
};

#endif // DOWNLOADER
