#ifndef GRAPHICSHALOEFFECT_P_H
#define GRAPHICSHALOEFFECT_P_H

// graphicshaloeffect_p.h
// 5/3/2012
// See: gui/effects/qgraphicseffect_p.h

#include "module/graphicseffect/graphicseffect_p.h"
#include "module/pixmapfilter/pixmaphalofilter.h"

class GraphicsHaloEffectPrivate : public QGraphicsEffectPrivate
{
  Q_DECLARE_PUBLIC(GraphicsHaloEffect)
public:
  GraphicsHaloEffectPrivate() : filter(new PixmapHaloFilter), opacity(1.0) {}
  ~GraphicsHaloEffectPrivate() { delete filter; }

  PixmapHaloFilter *filter;
  qreal opacity;
};

#endif // GRAPHICSHALOEFFECT_P_H
