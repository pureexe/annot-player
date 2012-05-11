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

  QList<AnnotationCodec*> codecs_;

public:
  enum Site { Bilibli = 0, Acfun, Nicovideo, SiteCount };

  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit AnnotationCodecManager(QObject *parent = 0);

signals:
  void error(QString msg);
  void message(QString msg);

  void fetched(AnnotationList annots, QString url);

public:
  int match(const QString &url) const;

public slots:
  void fetch(int id, const QString &url);
  bool fetch(const QString &url)
  {
    int r = match(url);
    if (r < 0)
      return false;
    fetch(r, url);
    return true;
  }
};

#endif // ANNOTATIONCODECMANAGER_H
