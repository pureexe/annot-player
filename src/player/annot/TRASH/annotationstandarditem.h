#ifndef ANNOTATIONSTANDARDITEM_H
#define ANNOTATIONSTANDARDITEM_H

// annotationsitem.h
// 10/4/2011

#include "core/universe/annotation.h"
#include <QtCore/QStandardItem>

class AnnotationStandardItem : public QStandardItem
{
  typedef AnnotationStandardItem Self;
  typedef QStandardItem Base;

  typedef Core::Universe::Annotation Annotation;

  Annotation annotation_;

public:
  AnnotationStandardItem();
  explicit AnnotationStandardItem(const Annotation &annot);

  const Annotation &annotation() const;
  void setAnnotation(const Annotation &annot);
};

#endif // ANNOTATIONSTANDARDITEM_H
