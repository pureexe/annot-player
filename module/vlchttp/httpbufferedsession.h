#ifndef HTTPBUFFEREDSESSION_H
#define HTTPBUFFEREDSESSION_H

// httpbufferedsession.h
// 2/21/2012

#include "module/vlchttp/httpsession.h"
#include "module/qtext/stoppable.h"
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtCore/QUrl>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class HttpBufferedSession : public VlcHttpSession
{
  Q_OBJECT
  Q_DISABLE_COPY(HttpBufferedSession)
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
  explicit HttpBufferedSession(const QUrl &url, QObject *parent = 0)
    : Base(parent), url_(url), nam_(0), reply_(0),
      pos_(0), size_(0), ready_(false) { }

  ~HttpBufferedSession();

  // - Properties -
public:
  const QUrl &url() const { return url_; }
  virtual QString contentType() const { return contentType_; } ///< \reimp
  virtual qint64 read(char *data, qint64 maxSize); ///< \reimp
  virtual bool seek(qint64 pos); ///< \reimp
  virtual qint64 size() const { return size_; } ///< \reimp
  virtual qint64 pos() const { return pos_; } ///< \reimp
  virtual qint64 availableSize() const { return buffer_.size(); } ///< \reimp

public slots:
  void setUrl(const QUrl &url) { url_ = url; }
  virtual void run(); ///< \reimp
  virtual void stop(); ///< \reimp

  virtual void waitForReady(); ///< \reimp
  virtual void waitForStopped(); ///< \reimp

protected slots:
  virtual void save(); ///< \reimp

  void updateSize();
  void updateContentType();
  void updateFileName();
  void finish();
  void waitForReplyReady();
protected:
  bool tryRedirect();
};

#endif // HTTPBUFFEREDSESSION_H
