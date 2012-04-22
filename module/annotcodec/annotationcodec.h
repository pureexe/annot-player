#ifndef ANNOTATIONCODEC_H
#define ANNOTATIONCODEC_H

// annotationcodec.h
// 2/3/2012

#include "module/annotcloud/annotation.h"

class AnnotationCodec : public QObject
{
  Q_OBJECT
  typedef AnnotationCodec Self;
  typedef QObject Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

public:
  explicit AnnotationCodec(QObject *parent = 0)
    : Base(parent) { }

signals:
  void error(QString msg);
  void message(QString msg);

  void fetched(AnnotationList annots, QString url);

public:
  virtual bool match(const QString &url) const = 0;
  virtual void fetch(const QString &url) = 0;
};

#endif // ANNOTATIONCODEC_H
