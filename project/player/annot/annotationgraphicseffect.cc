// annotationgraphicseffect.cc
// 5/3/2012

#include "annotationgraphicseffect.h"

// - Construction -

AnnotationGraphicsEffect::AnnotationGraphicsEffect(QObject *parent)
  : Base(parent)
{
  enum { offset = 1, radius = 16 };
  setBlurRadius(radius); // in pixels
  setOffset(offset); // in pixels
  //setColor(Qt::black);
  //setColor(Qt::red);
  //setColor(HALO_COLOR);
  setOpacity(0.0);
}

// EOF
