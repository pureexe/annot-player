#ifndef YOUTUBEMRLRESOLVER_H
#define YOUTUBEMRLRESOLVER_H

// youtubemrlresolver.h
// 1/24/2011

#include "lib/mrlresolver/mrlresolver.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QUrl)

class YoutubeMrlResolver : public MrlResolver
{
  Q_OBJECT
  Q_DISABLE_COPY(YoutubeMrlResolver)
  typedef YoutubeMrlResolver Self;
  typedef MrlResolver Base;

  QNetworkAccessManager *nam_;

public:
  explicit YoutubeMrlResolver(QObject *parent = nullptr)
    : Base(parent) { init(); }

public:
  bool matchMedia(const QString &href) const override;

public slots:
  bool resolveMedia(const QString &href) override;
protected slots:
  void resolveMedia(QNetworkReply *reply);

private:
  void init();
  static QString formatTitle(const QString &text);
  static QString decodeYoutubeUrl(const QUrl &url);
};

#endif // YOUTUBEMRLRESOLVER_H
