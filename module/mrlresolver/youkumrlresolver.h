#ifndef YOUKUMRLRESOLVER_H
#define YOUKUMRLRESOLVER_H

// youkumrlresolver.h
// 1/24/2011

#include "mrlresolver.h"
#include <QString>

QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class YoukuMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef YoukuMrlResolver Self;
  typedef MrlResolver Base;

  QNetworkAccessManager *qnam_;
  QString preferredType_; // should be one of "mp4", "flv", or "flvhd"

public:
  explicit YoukuMrlResolver(QObject *parent = 0);

public:
  bool matchMedia(const QString &href) const; ///< \override

public slots:
  void resolveMedia(const QString &href); ///< \override

  void setPreferredType(const QString &type) { preferredType_ = type; }

protected slots:
  void resolveMedia(QNetworkReply *reply);

private:
  static QString newSid();
  static QString decodeFileIdAlphabet(long seed);
  static QString decodeFileId(QString fileId, long seed);
};

#endif // YOUKUMRLRESOLVER_H
