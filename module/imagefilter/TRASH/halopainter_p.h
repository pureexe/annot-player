#ifndef HALOPAINTER_H
#define HALOPAINTER_H

#include <QCache>
#include <QPixmap>

class QRect;
class QPainter;

namespace Plasma {

class TileSet
{
public:
  enum Tile {
    Left, Center, Right, NTiles
  };

  TileSet(const QPixmap &pixmap);
  ~TileSet() {}

  void paint(QPainter *painter, const QRect &rect);

protected:
  QPixmap tiles[NTiles];
};


// -----------------------------------------------------------------------


class HaloPainter : public QObject
{
public:
  ~HaloPainter();
  static HaloPainter *instance() {
    if (!s_instance) {
      s_instance = new HaloPainter;
    }
    return s_instance;
  }

  static void drawHalo(QPainter *painter, const QRect &textRect) {
    instance()->paint(painter, textRect);
  }

private:
  HaloPainter();
  TileSet *tiles(int height) const;
  void paint(QPainter *painter, const QRect &textRect) const;

private:
  static HaloPainter *s_instance;
  mutable QCache<int, TileSet> m_tileCache;
  mutable QCache<int, QPixmap> m_haloCache;
};

} // namespace Plasma

#endif

