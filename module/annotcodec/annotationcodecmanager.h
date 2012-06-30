#ifndef ANNOTATIONCODECMANAGER_H
#define ANNOTATIONCODECMANAGER_H

// annotationcodecmanager.h
// 2/3/2012

#include "module/annotcodec/annotationcodec.h"

class AnnotationCodecManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationCodecManager)
  typedef AnnotationCodecManager Self;
  typedef QObject Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  QList<AnnotationCodec *> codecs_;

public:
  enum Format { UnknownFormat = -1, Nicovideo = 0, Acfun, Bilibili, FormatCount };

  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit AnnotationCodecManager(QObject *parent = 0);

signals:
  void error(QString msg);
  void message(QString msg);

  void fetched(AnnotationList annots, QString url, QString originalUrl);

public:
  int match(const QString &url) const;

  static AnnotationList parseFile(const QString &fileName);
  static Format fileFormat(const QString &fileName);

  static bool isAnnotatedFile(const QString &fileName);
  static QString parseAnnotatedUrl(const QString &fileName);

protected:
  static QString parseAnnotatedHeader(const QString &fileName);

public slots:
  void fetch(int id, const QString &url, const QString &originalUrl);
  bool fetch(const QString &url, const QString &originalUrl)
  {
    int r = match(url);
    if (r < 0)
      return false;
    fetch(r, url, originalUrl);
    return true;
  }
};

#endif // ANNOTATIONCODECMANAGER_H
