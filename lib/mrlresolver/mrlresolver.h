#ifndef MRLRESOLVER_H
#define MRLRESOLVER_H

// mrlresolver.h
// 1/24/2011

#include "lib/mrlresolver/mrlinfo.h"
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MrlResolver)
  typedef MrlResolver Self;
  typedef QObject Base;

  bool sync_;

  QNetworkCookieJar *resolvedCookieJar_;
  MediaInfo resolvedMediaInfo_;
  QString resolvedSubtitleUrl_;

public:
  explicit MrlResolver(QObject *parent = nullptr)
    : Base(parent), sync_(false), resolvedCookieJar_(nullptr) { }

signals:
  void message(const QString &text);
  void errorMessage(const QString &text);
  void synchronizedChanged(bool t);

  void mediaResolved(MediaInfo mi, QNetworkCookieJar *cookieJar = nullptr);
  void subtitleResolved(QString suburl, QString refurl, QString title);

public:
  virtual bool matchMedia(const QString &) const { return false; }
  virtual bool matchSubtitle(const QString &) const { return false; }

  QNetworkCookieJar *resolvedCookieJar() const { return resolvedCookieJar_; }
  const MediaInfo &resolvedMediaInfo() const { return resolvedMediaInfo_; }
  const QString &resolvedSubtitleUrl() const { return resolvedSubtitleUrl_; }

  bool isSynchronized() const { return sync_; }
public slots:
  void setSynchronized(bool t) { emit synchronizedChanged(sync_ = t); }
  virtual bool resolveMedia(const QString &)  { return false; }

  virtual bool resolveSubtitle(const QString &) { return false; }

protected:
  void setResolvedCookieJar(QNetworkCookieJar *jar) { resolvedCookieJar_ = jar; }
  void setResolvedMediaInfo(const MediaInfo &mi) { resolvedMediaInfo_ = mi; }
  void setResolvedSubtitleUrl(const QString &url) { resolvedSubtitleUrl_ = url; }
};

#endif // MRLRESOLVER_H
