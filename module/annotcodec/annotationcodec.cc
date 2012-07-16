// annotationcodec.cc
// 6/28/2012

#include "module/annotcodec/annotationcodec.h"
#include <QtCore/QByteArray>

// - Helpers -

QByteArray
AnnotationCodec::skipLeadingCommentAndNewLine(const QByteArray &data, const char *start, const char *stop)
{
  Q_ASSERT(start);
  Q_ASSERT(stop);
  if (!data.startsWith(start))
    return data;
  int pos = data.indexOf(stop);
  if (pos < 0)
    return data;
  int len = qstrlen(stop);
  while (data.size() > pos+len  &&
         data[pos+len] == '\n')
    pos++;
  return data.mid(pos+len);
}

// EOF
