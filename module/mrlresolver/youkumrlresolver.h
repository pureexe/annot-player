#ifndef YOUKUMRLRESOLVER_H
#define YOUKUMRLRESOLVER_H

// youkumrlresolver.h
// 1/24/2011

#include "mrlresolver.h"

QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class YoukuMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef YoukuMrlResolver Self;
  typedef MrlResolver Base;

  QNetworkAccessManager *qnam_;

public:
  explicit YoukuMrlResolver(QObject *parent = 0);

public:
  bool match(const QString &href) const; ///< \override

public slots:
  void resolve(const QString &href); ///< \override

protected slots:
  void resolve(QNetworkReply *reply);

private:
  static QString newSid();
  static QString decodeFileIdAlphabet(long seed);
  static QString decodeFileId(QString fileId, long seed);
};

#endif // YOUKUMRLRESOLVER_H
