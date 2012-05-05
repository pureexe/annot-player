#ifndef _CORE_ANNOTATION_H
#define _CORE_ANNOTATION_H

// core/annotation.h
// 7/4/2011
// Describe annotation.

#include "core/text.h"
#include <QDateTime>

namespace Core {
  class User;

  // Annotation
  /**
   *  \brief  Annotation.
   *
   *  Annotation categories (annotated objects):
   *  - To Document
   *  - To Page
   *  - To Position
   *  - To Text (TextRectList)
   *  - To Annotation -- reply
   *
   *  PositinAnnot.style
   *  - Position
   *
   *  TextAnnot.style (highlight styles):
   *  - Background color
   *  - Highlighted keywords and color
   *
   *  Note: always float!! allow application to float
   */
  class Annotation : public QObject
  {
    Q_OBJECT
    //Q_DISABLE_COPY(Annotation)
    typedef Annotation Self;
    typedef QObject Base;

  public:
    explicit Annotation(QObject *parent = 0) : Base(parent) { }

  public:
    // Meta data
    int id() const { return 0; }
    //int userid() const;
    const User *user() const { return 0; }
    QDateTime created() const { return QDateTime(); }
    QDateTime modified() const { return QDateTime(); }


    // Document ID
    int document();
    int pageNum();
    const TextRectList &location() const;

    // Contents
    const QString &title() const;
    const QString &contents() const;

    // Render style:
    // Avoid hardcode style, but hint indicating type of annotation and how to render the annotation
    //annotation_type
    //highlight_type


    // Extra information, statistis
    //AnnotationList *replies() const { return 0; }
    //int votes

  };

} // namespace Core

#endif // _CORE_ANNOTATION_H
