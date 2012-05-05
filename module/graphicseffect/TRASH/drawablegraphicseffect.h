#ifndef DRAWABLEGRAPHICSEFFECT_H
#define DRAWABLEGRAPHICSEFFECT_H

// drawablegraphicseffect.h
// 5/3/2012

#include "module/graphicseffect/drawable.h"
#include <QtGlobal>

#include <QtGui/QGraphicsEffect>

class DrawableGraphicsEffect : public QGraphicsEffect
{
  Q_OBJECT
  Q_DISABLE_COPY(DrawableGraphicsEffect)
  typedef DrawableGraphicsEffect Self;
  typedef QGraphicsEffect Base;

  QList<Drawable *> d_;

public:
  explicit DrawableGraphicsEffect(QObject *parent = 0)
    : Base(parent) { }

  ~DrawableGraphicsEffect() { clear(); }

public:
  void add(Drawable *d) { Q_ASSERT(d); d_.append(d); }

  void clear()
  {
    if (!d_.isEmpty()) {
      foreach (Drawable *d, d_)
        delete d;
      d_.clear();
    }
  }

public:
  virtual void draw(QPainter *painter) ///< \override
  {
    foreach (Drawable *d, d_)
      d->draw(painter);
  }
};

#include <QtGui/QGraphicsBlurEffect>
class DrawableGraphicsBlurEffect : public QGraphicsBlurEffect, public Drawable
{
  Q_OBJECT
  Q_DISABLE_COPY(DrawableGraphicsBlurEffect)
  typedef DrawableGraphicsBlurEffect Self;
  typedef QGraphicsBlurEffect Base;
public:
  explicit DrawableGraphicsBlurEffect(QObject *parent = 0)
    : Base(parent) { }
  void draw(QPainter *painter) { Base::draw(painter); } ///< \override
};

#include <QtGui/QGraphicsColorizeEffect>
class DrawableGraphicsColorizeEffect : public QGraphicsColorizeEffect, public Drawable
{
  Q_OBJECT
  Q_DISABLE_COPY(DrawableGraphicsColorizeEffect)
  typedef DrawableGraphicsColorizeEffect Self;
  typedef QGraphicsColorizeEffect Base;
public:
  explicit DrawableGraphicsColorizeEffect(QObject *parent = 0)
    : Base(parent) { }
  void draw(QPainter *painter) { Base::draw(painter); } ///< \override
};

#include <QtGui/QGraphicsDropShadowEffect>
class DrawableGraphicsDropShadowEffect : public QGraphicsDropShadowEffect, public Drawable
{
  Q_OBJECT
  Q_DISABLE_COPY(DrawableGraphicsDropShadowEffect)
  typedef DrawableGraphicsDropShadowEffect Self;
  typedef QGraphicsDropShadowEffect Base;
public:
  explicit DrawableGraphicsDropShadowEffect(QObject *parent = 0)
    : Base(parent) { }
  void draw(QPainter *painter) { Base::draw(painter); } ///< \override
};

#include <QtGui/QGraphicsOpacityEffect>
class DrawableGraphicsOpacityEffect : public QGraphicsOpacityEffect, public Drawable
{
  Q_OBJECT
  Q_DISABLE_COPY(DrawableGraphicsOpacityEffect)
  typedef DrawableGraphicsOpacityEffect Self;
  typedef QGraphicsOpacityEffect Base;
public:
  explicit DrawableGraphicsOpacityEffect(QObject *parent = 0)
    : Base(parent) { }
  void draw(QPainter *painter) { Base::draw(painter); } ///< \override
};

#endif // DRAWABLEGRAPHICSEFFECT_H
