#ifndef _VLCCORE_HTTP_H
#define _VLCCORE_HTTP_H

// vlccore/http.h
// 2/21/2012

#ifdef _MSC_VER
#  pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include <QObject>
#include <QString>
#include <QStringList>

#ifdef _MSC_VER
  typedef size_t ssize_t;
#endif // _MSC_VER

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

struct vlc_object_t;
struct access_t;

class VlcHttpSession;

class VlcHttpPlugin : public QObject
{
  Q_OBJECT
  typedef VlcHttpPlugin Self;
  typedef QObject Base;

  static VlcHttpSession *session_;
  static QNetworkCookieJar *cookieJar_;
  static QString url_;
  static QStringList urls_;
  static qint64 duration_;
  static QString mediaTitle_;
  static bool bufferSaved_;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit VlcHttpPlugin(QObject *parent = 0)
    : Base(parent) { }

signals:
  void error(const QString &msg);
  void message(const QString &msg);
  void fileSaved(const QString &fileName);
  void progress(qint64 receivedBytes, qint64 totalBytes);

public:
  static void load();
  static void unload();

  static bool isBufferSaved() { return bufferSaved_; }

  static void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }
  static void setBufferSaved(bool t);
  static void setUrls(const QStringList &urls) { urls_ = urls; }
  static void setDuration(qint64 msec) { duration_ = msec; }
  static void setMediaTitle(const QString &title) { mediaTitle_ = title; }

  static void closeSession();

  static void save();
  static bool isFinished();

  static qint64 duration();
  static qint64 availableDuration();

  static qint64 size();
  static qint64 availableSize();

protected:
  // - Plugin APIs -
  static int open(vlc_object_t *);
  static void close(vlc_object_t *);
  static ssize_t read(access_t *, quint8 *, size_t);
  //static ssize_t readCompressed(access_t *, quint8 *, size_t); // TODO: not implemented
  static int seek(access_t *, quint64);
  static int control(access_t *, int, va_list);

  static void openSession();
};

#endif // _VLCCORE_HTTP_H
