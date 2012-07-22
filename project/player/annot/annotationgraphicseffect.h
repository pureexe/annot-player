#ifndef ANNOTATIONGRAPHICSEFFECT_H
#define ANNOTATIONGRAPHICSEFFECT_H

// annotationgraphicseffect.h
// 5/3/2012

// Original Qt implementation for drop shadow
//#include "module/graphicseffect/graphicsshadoweffect.h"
//typedef GraphicsShadowEffect AnnotationGraphicsEffectBase;

// Faster and with thicker color
#include "module/graphicseffect/graphicshaloeffect.h"
typedef GraphicsHaloEffect AnnotationGraphicsEffectBase;

class AnnotationGraphicsEffect : public AnnotationGraphicsEffectBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsEffect)
  typedef AnnotationGraphicsEffect Self;
  typedef AnnotationGraphicsEffectBase Base;

public:
  explicit AnnotationGraphicsEffect(QObject *parent = nullptr);
};

#endif // ANNOTATIONGRAPHICSEFFECT_H
