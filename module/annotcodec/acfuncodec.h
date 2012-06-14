#ifndef ACFUNCODEC_H
#define ACFUNCODEC_H

// acfuncodec.h
// 2/4/2012

#include "module/annotcodec/annotationcodec.h"
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)

class AcfunCodec : public AnnotationCodec
{
  Q_OBJECT
  Q_DISABLE_COPY(AcfunCodec)
  typedef AcfunCodec Self;
  typedef AnnotationCodec Base;

  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  QNetworkAccessManager *qnam_;
  QHash<QString, int> retries_;

public:
  enum Format { UnknownFormat = 0, Json, Xml };

  explicit AcfunCodec(QObject *parent = 0);

public:
  virtual bool match(const QString &url) const; ///< \reimp
  virtual void fetch(const QString &url); ///< \reimp

public:
  static AnnotationList parseFile(const QString &fileName, Format f = UnknownFormat)
  {
    switch (f) {
    case Xml: return parseXmlFile(fileName);
    case Json: return parseJsonFile(fileName);
    default:
      {
        QString suffix = QFileInfo(fileName).suffix();
        if (!suffix.compare("xml", Qt::CaseInsensitive))
          return parseXmlFile(fileName);
        else if (!suffix.compare("json", Qt::CaseInsensitive))
          return parseJsonFile(fileName);
        else {
          QFile f(fileName);
          return f.open(QIODevice::ReadOnly) ? parseDocument(f.readAll(), UnknownFormat) : AnnotationList();
        }
      }
    }
  }

  static AnnotationList parseDocument(const QByteArray &data, Format f = UnknownFormat)
  {
    switch (f) {
    case Xml: return parseXmlDocument(data);
    case Json: return parseJsonDocument(data);
    default:
      if (data.size() < 4)
        return AnnotationList();
      {
        QString t = QString::fromLocal8Bit(data.data(), 4);
        if (t.isEmpty())
          return AnnotationList();
        switch (t[0].toAscii()) {
        case '<': return parseXmlDocument(data);
        default:  return parseJsonDocument(data);
        }
      }
    }
  }

protected:

  static AnnotationList parseXmlFile(const QString &fileName)
  {
    QFile f(fileName);
    return f.open(QIODevice::ReadOnly) ? parseXmlDocument(f.readAll()) : AnnotationList();
  }

  static AnnotationList parseJsonFile(const QString &fileName)
  {
    QFile f(fileName);
    return f.open(QIODevice::ReadOnly) ? parseJsonDocument(f.readAll()) : AnnotationList();
  }

  static AnnotationList parseXmlDocument(const QByteArray &data);
  static AnnotationList parseJsonDocument(const QByteArray &data);

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
