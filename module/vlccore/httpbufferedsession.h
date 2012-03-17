#ifndef _VLCCORE_HTTPBUFFEREDSESSION_H
#define _VLCCORE_HTTPBUFFEREDSESSION_H

// vlccore/httpbufferedsession.h
// 2/21/2012

#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "module/vlccore/httpsession.h"
#include "module/qtext/stoppable.h"
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class HttpBufferedSession : public VlcHttpSession
{
  Q_OBJECT
  typedef HttpBufferedSession Self;
  typedef VlcHttpSession Base;

  QUrl url_;
  QNetworkAccessManager *nam_;
  QNetworkReply *reply_;

  QByteArray buffer_;
  qint64 pos_, size_;

  QMutex m_;
  QWaitCondition readyCond_, readyReadCond_, stoppedCond_;
  bool ready_;

  QString contentType_, fileName_;

public:
  explicit HttpBufferedSession(QObject *parent = 0)
    : Base(parent), nam_(0), reply_(0),
      pos_(0), size_(0), ready_(false) { }

  explicit HttpBufferedSession(const QUrl &url, QObject *parent = 0)
    : Base(parent), url_(url), nam_(0), reply_(0),
      pos_(0), size_(0), ready_(false) { }

  ~HttpBufferedSession();

signals:
  void error(QString msg);
  void message(QString msg);
  void fileSaved(QString msg);
  void progress(qint64 receivedBytes, qint64 totalBytes);

  // - Properties -
public:
  const QUrl &url() const { return url_; }
  virtual QString contentType() const { return contentType_; } ///< \override
  virtual qint64 read(char *data, qint64 maxSize); ///< \override
  virtual bool seek(qint64 pos); ///< \override
  virtual qint64 size() const { return size_; } ///< \override
  virtual qint64 pos() const { return pos_; } ///< \override
  virtual qint64 availableSize() const { return buffer_.size(); } ///< \override

public slots:
  void setUrl(const QUrl &url) { url_ = url; }
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  virtual void waitForReady(); ///< \override
  virtual void waitForStopped(); ///< \override

protected slots:
  void invalidateSize();
  void invalidateContentType();
  void invalidateFileName();
  void finish();
  void save();
  void waitForReplyReady();
protected:
  bool tryRedirect();
};

#endif // _VLCCORE_HTTPBUFFEREDSESSION_H
