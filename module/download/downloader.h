#ifndef DOWNLOADER
#define DOWNLOADER

// downloader.h
// 2/3/2012

#include <QObject>
#include <QString>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class Downloader : public QObject
{
  Q_OBJECT
  typedef Downloader Self;
  typedef QObject Base;

  int state_;
  QNetworkAccessManager *qnam_;
  QString path_;
  bool zipped_;

public:
  enum State { Error = -1, OK = 0, Downloading = 1 };

public:
  explicit Downloader(const QString &path, bool zipped = false, QObject *parent = 0);

  int state() const { return state_; }

public slots:
  void start(const QUrl &url, bool async = true);

protected slots:
  void save(QNetworkReply *reply);
  static bool save(const QByteArray &data, const QString &path);
};

#endif // DOWNLOADER
