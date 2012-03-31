#ifndef _VLCCORE_HTTPSESSION_H
#define _VLCCORE_HTTPSESSION_H

// vlccore/httpsession.h
// 2/21/2012

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "module/qtext/stoppable.h"
#include <QString>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class VlcHttpSession : public StoppableThread
{
  Q_OBJECT
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
  void fileSaved(QString msg);
  void progress(qint64 receivedBytes, qint64 totalBytes);

  // - Properties -
public:
  QNetworkCookieJar *cookieJar() const { return cookieJar_; }
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

  virtual void save() { }

  void setCookieJar(QNetworkCookieJar *jar) { cookieJar_ = jar; }
public slots:
  void setBufferSaved(bool t) { saveBuffer_ = t; }

  void setMediaTitle(const QString &title) { mediaTitle_ = title; }

  virtual void waitForReady() = 0;
  virtual void waitForStopped() = 0;

protected slots:
  void setState(State state) { state_ = state; }

  static bool isMultiMediaMimeType(const QString &contentType);

private:
  State state_;
  bool saveBuffer_;

  QNetworkCookieJar *cookieJar_;
  QString mediaTitle_;
};

#endif // _VLCCORE_HTTPSESSION_H
