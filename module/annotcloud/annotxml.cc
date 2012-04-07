// annotcloud/annotxml.cc
// 3/21/2012

#include "module/annotcloud/annotxml.h"
#include <QFile>

QDomDocument
AnnotCloud::
AnnotationXmlParser::toDomDocument(const AnnotationList &annots) const
{
  Q_UNUSED(annots);
  // CHECKPOINT
  QDomDocument doc;
  return doc;
}

bool
AnnotCloud::
AnnotationXmlParser::writeXml(const QString &fileName, const AnnotationList &annots) const
{
  // Using QXmlStreamWriter would be more efficient
  QDomDocument doc = toDomDocument(annots);
  if (doc.isNull())
    return false;

  QFile f(fileName);
  if (!f.open(QIODevice::WriteOnly))
    return false;
  f.write(doc.toByteArray());
  return f.error() == QFile::NoError;
}

// EOF
