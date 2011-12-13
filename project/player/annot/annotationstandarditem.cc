// annotationstandarditem.cc
// 10/5/2011

#include "annotationstandarditem.h"
#include <QtGui>

using namespace Core::Universe;

//#define DEBUG "annotationstandarditem"
#include "module/debug/debug.h"

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
