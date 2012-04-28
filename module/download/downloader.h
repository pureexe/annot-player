#ifndef DOWNLOADER_H
#define DOWNLOADER_H

// downloader.h
// 2/3/2012

#include <QtCore/QHash>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QUrl;
QT_END_NAMESPACE

class DownloaderController : public QObject
{
  Q_OBJECT
  typedef DownloaderController Self;
  typedef QObject Base;
public:
  static Self *globalController() { static Self g; return &g; }
protected:
  explicit DownloaderController(QObject *parent = 0) : Base(parent) { }

signals:
  void message(QString);
  void error(QString);
  void warning(QString);
  void notification(QString);
  void aborted();

public slots:
  void abort() { emit aborted(); }
};

class Downloader : public QObject
{
  Q_OBJECT
  typedef Downloader Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;
  QNetworkReply *reply_;
  int state_;
  QString fileName_;
  bool stopped_;

public:
  enum State { Error = -1, OK = 0, Downloading = 1 };

public:
 explicit Downloader(const QString &fileName, QObject *parent = 0)
    : Base(parent), nam_(0), reply_(0), state_(OK), fileName_(fileName), stopped_(false) { init(); }
 explicit Downloader(QObject *parent = 0)
    : Base(parent), nam_(0), reply_(0), state_(OK), stopped_(false) { init(); }

  int state() const { return state_; }

  QNetworkAccessManager *networkAccessManager() { return nam_; }

private:
  void init();

signals:
  void progress(qint64 bytesReceived, qint64 bytesTotal);
  void finished();
  void error(QString);
  void message(QString);
  void warning(QString);
  void notification(QString);

public slots:
  void abort();

  void setFileName(const QString &fileName) { fileName_ = fileName; }
  void get(const QNetworkRequest &req, bool async = true, int retries = 5);
  void get(const QUrl &url,
           const QString &header = QString(), bool async = true, int retries = 5);
  void post(const QUrl &url, const QByteArray &data = QByteArray(),
            const QString &header = QString(), bool async = true, int retries = 5);

  QNetworkAccessManager *networkAccessManager() const { return nam_; }

  void stop() { stopped_ = true; }

protected slots:
  void save(QNetworkReply *reply);
  static bool save(const QByteArray &data, const QString &fileName);

  static QHash<QString, QString> parseHttpHeader(const QString &header);

protected:
  bool tryRedirect(QNetworkReply *reply);
  static QByteArray encodeUrlParameters(const QString &params);
};

#endif // DOWNLOADER_H
