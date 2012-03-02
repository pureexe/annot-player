#ifndef _VLCCORE_HTTP_H
#define _VLCCORE_HTTP_H

// vlccore/http.h
// 2/21/2012

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QUrl>
#include "module/qtext/stoppable.h"
#include <cstdarg>

#ifdef _MSC_VER
  typedef size_t ssize_t;
#endif // _MSC_VER

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

struct vlc_object_t;
struct access_t;

class VlcHttpSession : public QThread, public Stoppable
{
  Q_OBJECT
  typedef VlcHttpSession Self;
  typedef QThread Base;

  enum State { Error = -1, Stopped = 0, Running, Finished };
  State state_;

  QUrl url_;
  QNetworkAccessManager *nam_;
  QNetworkCookieJar *cookieJar_;
  QNetworkReply *reply_;

  QByteArray buffer_;
  qint64 pos_, size_;
  QString contentType_;

  QMutex m_;
  QWaitCondition readyCond_, readyReadCond_, stoppedCond_;
  bool ready_;

  QString mediaTitle_, fileName_;

public:
  explicit VlcHttpSession(QObject *parent = 0);
  ~VlcHttpSession();

signals:
  void error(QString msg);
  void message(QString msg);
  void fileSaved(QString msg);
  void progress(qint64 receivedBytes, qint64 totalBytes);

  // - Properties -
public:
  QString contentType() const { return contentType_; }
  const QUrl &url() const { return url_; }
  qint64 read(char *data, qint64 maxSize);
  bool seek(qint64 pos);
  qint64 size() const { return size_; }
  qint64 pos() const { return pos_; }
  qint64 availableSize() const { return buffer_.size(); }

  bool isRunning() const { return state_ == Running; }
  bool isStopped() const { return state_ == Stopped; }
  bool isFinished() const { return state_ == Finished; }

  void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }
  void setMediaTitle(const QString &title) { mediaTitle_ = title; }

public slots:
  void setUrl(const QUrl &url) { url_ = url; }
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  void waitForReady();
  void waitForStopped();

protected slots:
  void invalidateSize();
  void invalidateContentType();
  void invalidateFileName();
  void finish();
  void save();
  void waitForReplyReady();
  void setState(State state) { state_ = state; }
protected:
  bool tryRedirect();
  static QString escapeFileName(const QString &name);
  static bool isMultiMediaMimeType(const QString &contentType);
};

class VlcHttpPlugin : public QObject
{
  Q_OBJECT
  typedef VlcHttpPlugin Self;
  typedef QObject Base;

  static QNetworkCookieJar *cookieJar_;
  static QString mediaTitle_;
public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit VlcHttpPlugin(QObject *parent = 0) : Base(parent) { }

signals:
  void error(const QString &msg);
  void message(const QString &msg);
  void fileSaved(const QString &fileName);
  void progress(qint64 receivedBytes, qint64 totalBytes);

public:
  static void load();
  static void unload();

  static void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }
  static void setMediaTitle(const QString &title) { mediaTitle_ = title; }

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
