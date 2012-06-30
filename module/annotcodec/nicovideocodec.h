#ifndef NICOVIDEOCODEC_H
#define NICOVIDEOCODEC_H

// nicovideocodec.h
// 2/6/2012

#include "module/annotcodec/annotationcodec.h"
#include <QFile>

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
  virtual bool match(const QString &url) const; ///< \reimp
  virtual void fetch(const QString &url, const QString &originalUrl); ///< \reimp

  void fetchLocalFile(const QString &path, const QString &originalUrl);

public:
  static AnnotationList parseFile(const QString &fileName)
  {
    QFile f(fileName);
    return f.open(QIODevice::ReadOnly) ? parseDocument(f.readAll()) : AnnotationList();
  }

  static AnnotationList parseDocument(const QByteArray &data);
protected:
  static QString parseText(const QString &text);
  static QString parsePrefix(const QString &text);

  static QByteArray skipXmlLeadingComment(const QByteArray &data)
  { return skipLeadingCommentAndNewLine(data, "<!--", "-->"); }
};

#endif // ANNOTATIONCODEC_H
