#ifndef HTTPPLUGIN_H
#define HTTPPLUGIN_H

// httpplugin.h
// 2/21/2012

#ifdef _MSC_VER
# pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

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
  Q_DISABLE_COPY(VlcHttpPlugin)
  typedef VlcHttpPlugin Self;
  typedef QObject Base;

  static bool loaded_;
  static VlcHttpSession *session_;
  static QNetworkCookieJar *cookieJar_;
  static QString url_;
  static QString originalUrl_;
  static QStringList urls_;
  static qint64 duration_;
  static QString mediaTitle_;
  static bool bufferSaved_;
  static QString cacheDirectory_;

  //typedef int (*pf_activate_t)(vlc_object_t *);
  //typedef void (*pf_deactivate_t)(vlc_object_t *);
  //static pf_activate_t pf_activate;
  //static pf_deactivate_t pf_deactivate;
  //static bool activated_;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit VlcHttpPlugin(QObject *parent = nullptr)
    : Base(parent) { }

signals:
  void message(const QString &msg);
  void errorMessage(const QString &msg);
  void warning(const QString &msg);
  void fileSaved(const QString &fileName);
  void progress(qint64 receivedBytes, qint64 totalBytes);
  void buffering();

public:
  static void setNetworkCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }

  static void load();
  static void unload();

  static bool isBufferSaved() { return bufferSaved_; }
  static QString cacheDirectory() { return cacheDirectory_; }

  static void setCacheDirectory(const QString &path) { cacheDirectory_ = path; }
  static void setBufferSaved(bool t);
  static void setOriginalUrl(const QString &url) { originalUrl_ = url; }
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

#endif // HTTPPLUGIN_H
