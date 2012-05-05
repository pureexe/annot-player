#ifndef NICOVIDEOCODEC_H
#define NICOVIDEOCODEC_H

// nicovideocodec.h
// 2/6/2012

#include "annotationcodec.h"

class NicovideoCodec : public AnnotationCodec
{
  Q_OBJECT
  Q_DISABLE_COPY(NicovideoCodec)
  typedef NicovideoCodec Self;
  typedef AnnotationCodec Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

public:
  explicit NicovideoCodec(QObject *parent = 0)
    : Base(parent) { }

public:
  virtual bool match(const QString &url) const; ///< \override
  virtual void fetch(const QString &url); ///< \overrride

  void fetchLocalFile(const QString &path);

  static AnnotationList parseDocument(const QByteArray &data);
protected:
  static QString parseText(const QString &text);
  static QString parsePrefix(const QString &text);
};

#endif // ANNOTATIONCODEC_H
