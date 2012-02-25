#ifndef _VLCCORE_HTTP_H
#define _VLCCORE_HTTP_H
#include <QMutex>
#include <QWaitCondition>

// vlccore/http.h
// 2/21/2012

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include <QObject>
#include <QString>
#include <QRunnable>
#include "module/qtext/stoppable.h"
#include <cstdarg>

#ifdef _MSC_VER
  typedef size_t ssize_t;
#endif // _MSC_VER

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

struct vlc_object_t;
struct access_t;
class RemoteStream;

class VlcHttpSession : public QObject, public QRunnable, public Stoppable
{
  Q_OBJECT
  typedef VlcHttpSession Self;
  typedef QObject Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  QString url_;
  RemoteStream *in_;

public:
  explicit VlcHttpSession(QObject *parent = 0);
  ~VlcHttpSession();
bool debug();
signals:
  void error(QString msg);
  void readyRead();
  void finished();
  void stopped();

  // - Properties -
public:
  QString contentType() const;
  const QString &url() const { return url_; }
  qint64 read(char *data, qint64 maxSize);
  bool seek(qint64 pos);
  qint64 size() const;
  qint64 pos() const;

  bool isRunning() const { return state_ == Running; }
  bool isStopped() const { return state_ == Stopped; }
  bool isFinished() const { return state_ == Finished; }

public slots:
  void setUrl(const QString &url) { url_ = url; }
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

protected slots:
  void finish();
  void abort();
};

class VlcHttpPlugin
{
  static QNetworkCookieJar *cookieJar_;
public:
  static void load();
  static void unload();

  static void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }
  static QNetworkCookieJar *cookieJar() { return cookieJar_; }

protected:
  // - Plugin APIs -
  static int open(vlc_object_t *);
  static void close(vlc_object_t *);
  static ssize_t read(access_t *, quint8 *, size_t);
  //static ssize_t readCompressed(access_t *, quint8 *, size_t); // TODO: not implemented
  static int seek(access_t *, quint64);
  static int control(access_t *, int, va_list);
};

#endif // _VLCCORE_VIDEO_H
