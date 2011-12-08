// annotationstandarditem.cc
// 10/5/2011

#include "annotationstandarditem.h"
#include <QtGui>

using namespace Core::Universe;

//#define DEBUG

#ifdef DEBUG
  #define DOUT(_msg)    qDebug() << "AnnotationStandardItem:" << _msg
#else
  #define DOUT(_dummy)
#endif // DEBUG

// - Constructions -

AnnotationStandardItem::AnnotationStandardItem()
{ }

AnnotationStandardItem::AnnotationStandardItem(const Annotation &annot)
{
  setAnnotation(annot);
}

const Annotation&
AnnotationStandardItem::annotation() const
{ return annotation_; }

void
AnnotationStandardItem::setAnnotation(const Annotation &annot)
{
  annotation_ = annot;
}

// EOF
