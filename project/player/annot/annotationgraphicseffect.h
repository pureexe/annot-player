#ifndef ANNOTATIONGRAPHICSEFFECT_H
#define ANNOTATIONGRAPHICSEFFECT_H

// annotationgraphicseffect.h
// 5/3/2012

#include "module/graphicseffect/graphicshaloeffect.h"

class AnnotationGraphicsEffect : public GraphicsHaloEffect
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsEffect)
  typedef AnnotationGraphicsEffect Self;
  typedef GraphicsHaloEffect Base;

public:
  explicit AnnotationGraphicsEffect(QObject *parent = 0);

  static qreal defaultOpacity() {
    return
#ifdef Q_WS_WIN
      0.95
#else
      1.0
#endif // Q_WS_WIN
    ;
  }
};

#endif // ANNOTATIONGRAPHICSEFFECT_H
