#ifndef ANNOTATIONGRAPHICSEFFECT_H
#define ANNOTATIONGRAPHICSEFFECT_H

// annotationgraphicseffect.h
// 5/3/2012

//#include "module/graphicseffect/graphicshaloeffect.h"
// CHECKPOINT
#include <QtGui/QGraphicsDropShadowEffect>

class AnnotationGraphicsEffect : public QGraphicsDropShadowEffect
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsEffect)
  typedef AnnotationGraphicsEffect Self;
  typedef QGraphicsDropShadowEffect Base;

public:
  explicit AnnotationGraphicsEffect(QObject *parent = 0);
};

#endif // ANNOTATIONGRAPHICSEFFECT_H
