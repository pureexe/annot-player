#ifndef DRAWABLE_H
#define DRAWABLE_H

// drawable.h
// 5/3/2012

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QPainter)

class Drawable
{
public:
  virtual void draw(QPainter *painter) = 0; ///< supposed to override QGraphicsEffect::drawable
};

#endif // DRAWABLE_H
