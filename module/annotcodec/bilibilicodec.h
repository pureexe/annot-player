#ifndef BILIBILICODEC_H
#define BILIBILICODEC_H

// bilibilicodec.h
// 2/3/2012

#include "annotationcodec.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class BilibiliCodec : public AnnotationCodec
{
  Q_OBJECT
  typedef BilibiliCodec Self;
  typedef AnnotationCodec Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  QNetworkAccessManager *qnam_;

public:
  explicit BilibiliCodec(QObject *parent = 0);

public:
  virtual bool match(const QString &url) const; ///< \override
  virtual void fetch(const QString &url); ///< \override

  static AnnotationList parseDocument(const QByteArray &data);
protected:
  static Annotation parseAttribute(const QString &attr);
  static QString parseText(const QString &text);

  static Annotation parseComment(const QString &attr, const QString &text)
  {
    Annotation a = parseAttribute(attr);
    QString t = parseText(text);
    if (!a.hasText())
      a.setText(t);
    else if (!t.isEmpty())
      a.setText(a.text() + " " + t);
    return a;
  }

protected slots:
  void parseReply(QNetworkReply *reply);
};

#endif // ANNOTATIONCODEC_H
