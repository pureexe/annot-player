#ifndef HTTPBUFFEREDSESSION_P_H
#define HTTPBUFFEREDSESSION_P_H

// httpbufferedsession_p.h
// 2/21/2012

#include "lib/vlchttp/httpsession_p.h"
#include "qtx/qxstoppable.h"
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
  explicit HttpBufferedSession(const QUrl &url, QObject *parent = nullptr)
    : Base(parent), url_(url), nam_(nullptr), reply_(nullptr),
      pos_(0), size_(0), ready_(false) { }

  ~HttpBufferedSession();

  // - Properties -
public:
  const QUrl &url() const { return url_; }
  QString contentType() const override { return contentType_; }
  qint64 read(char *data, qint64 maxSize) override;
  bool seek(qint64 pos) override;
  qint64 size() const override { return size_; }
  qint64 pos() const override { return pos_; }
  qint64 availableSize() const override { return buffer_.size(); }

public slots:
  void setUrl(const QUrl &url) { url_ = url; }
  void run() override;
  void stop() override;

  void waitForReady() override;
  void waitForStopped() override;

protected slots:
  void save() override;

  void updateSize();
  void updateContentType();
  void updateFileName();
  void finish();
  void waitForReplyReady();
protected:
  bool tryRedirect();
};

#endif // HTTPBUFFEREDSESSION_H
