#ifndef YOUKUMRLRESOLVER_H
#define YOUKUMRLRESOLVER_H

// youkumrlresolver.h
// 1/24/2011

#include "lib/mrlresolver/mrlresolver.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class YoukuMrlResolver : public MrlResolver
{
  Q_OBJECT
  Q_DISABLE_COPY(YoukuMrlResolver)
  typedef YoukuMrlResolver Self;
  typedef MrlResolver Base;

  QString preferredType_; // should be one of "mp4", "flv", or "flvhd"

  QNetworkAccessManager *nam_;

public:
  explicit YoukuMrlResolver(QObject *parent = nullptr)
    : Base(parent) { init(); }

public:
  bool matchMedia(const QString &href) const override;

public slots:
  bool resolveMedia(const QString &href) override;

  void setPreferredType(const QString &type) { preferredType_ = type; }

protected slots:
  void resolveMedia(QNetworkReply *reply);

private:
  void init();
  static QString newSid();
  static QString decodeFileIdAlphabet(long seed);
  static QString decodeFileId(QString fileId, long seed);
};

#endif // YOUKUMRLRESOLVER_H
