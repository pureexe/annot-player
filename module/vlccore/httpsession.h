#ifndef _VLCCORE_HTTPSESSION_H
#define _VLCCORE_HTTPSESSION_H

// vlccore/httpsession.h
// 2/21/2012

#include "module/qtext/stoppable.h"
#include <QtCore/QString>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class VlcHttpSession : public StoppableThread
{
  Q_OBJECT
  Q_DISABLE_COPY(VlcHttpSession)
  typedef VlcHttpSession Self;
  typedef StoppableThread Base;

protected:
  enum State { Error = -1, Stopped = 0, Running, Finished };
public:
  explicit VlcHttpSession(QObject *parent = 0)
    : Base(parent), state_(Stopped), cookieJar_(0) { }

signals:
  void error(QString msg);
  void message(QString msg);
  void warning(QString msg);
  void fileSaved(QString msg);
  void progress(qint64 receivedBytes, qint64 totalBytes);

  // - Properties -
public:
  const QString &originalUrl() const { return originalUrl_; }
  virtual QString contentType() const { return QString(); }
  QString mediaTitle() const { return mediaTitle_; }
  virtual qint64 read(char *data, qint64 maxSize) = 0;
  virtual bool seek(qint64 pos) = 0;
  bool reset() { return seek(0); }
  virtual qint64 size() const = 0;
  virtual qint64 pos() const = 0;
  virtual qint64 availableSize() const = 0;

  virtual qint64 duration() const { return 0; }
  virtual qint64 availableDuration() const { return 0; }

  bool isBufferSaved() const { return saveBuffer_; }

  bool isRunning() const { return state_ == Running; }
  bool isStopped() const { return state_ == Stopped; }
  bool isFinished() const { return state_ == Finished; }

  const QString &cachePath() const { return cachePath_; }

  virtual void save() { }

  void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }

public slots:
  void setOriginalUrl(const QString &url) { originalUrl_ = url; }
  void setBufferSaved(bool t) { saveBuffer_ = t; }

  void setMediaTitle(const QString &title) { mediaTitle_ = title; }
  void setCachePath(const QString &path) { cachePath_ = path; }

  virtual void waitForReady() = 0;
  virtual void waitForStopped() = 0;

protected:
  QNetworkCookieJar *cookieJar() const { return cookieJar_; }

protected slots:
  void setState(State state) { state_ = state; }

  static bool isMultiMediaMimeType(const QString &contentType);

private:
  State state_;
  bool saveBuffer_;

  QString originalUrl_;
  QString cachePath_;
  QNetworkCookieJar *cookieJar_;
  QString mediaTitle_;
};

#endif // _VLCCORE_HTTPSESSION_H
