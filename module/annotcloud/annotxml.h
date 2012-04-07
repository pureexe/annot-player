#ifndef _ANNOTCLOUD_ANNOTXML_H
#define _ANNOTCLOUD_ANNOTXML_H

// annotcloud/annotxml.h
// 3/20/2012

#include "module/annotcloud/annotation.h"
#include <QObject>
#include <QString>
#include <QDomDocument>

namespace AnnotCloud {

class AnnotationXmlParser : public QObject
{
  Q_OBJECT
  typedef AnnotationXmlParser Self;
  typedef QObject Base;

public:
  static Self *globalInstance() { static Self g; return &g; }

  explicit AnnotationXmlParser(QObject *parent = 0)
    : Base(parent) { }

public:
  //AnnotationList fromDomDocument(const QDomDocument &doc) const;
  QDomDocument toDomDocument(const AnnotationList &annots) const;

  //bool readXml(const QString &fileName) const;
  bool writeXml(const QString &fileName, const AnnotationList &annots) const;
};

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_ANNOTXML_H
