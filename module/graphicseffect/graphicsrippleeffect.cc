// graphicsrippleeffect.cc
// 5/3/2012

#include "module/graphicseffect/graphicsrippleeffect.h"
#include <QtGui/QPainter>

#define DEBUG "graphicsrippleeffect"
#include "module/debug/debug.h"

// - Helper -

namespace { // anonymous

  /**
   * Allocates an integer matrix with the given size.
   * @param size Matrix size
   * @return integer matrix
   */
  inline int **allocateWaveMap(const QSize &size)
  {
    int **waveMap = new int *[size.width()];
    if (!waveMap) {
      DOUT("could not allocate wave map");
      return 0;
    }

    for (int x = 0; x < size.width(); ++x) {
      waveMap[x] = new int[size.height()];
      if (!waveMap[x]) {
        DOUT("could not allocate wave map");
        return 0;
      }
    }

    return waveMap;
  }

  /**
   * Deallocates an integer matrix
   * @param waveMap integer matrix
   */
  inline void deallocateWaveMap(int **waveMap)
  {
    if (waveMap) {
      delete [] *waveMap;
      delete [] waveMap;
    }
  }

} // anonymous namespace

GraphicsRippleEffect::GraphicsRippleEffect(QObject *parent)
  : QGraphicsEffect(parent),
    m_offset(1),
    m_damping(16),
    m_heigth(1),
    m_opacity(0.0),
    m_mapSize(boundingRect().size().toSize()),
    m_previousMap(0),
    m_currentMap(0)
{ }

GraphicsRippleEffect::~GraphicsRippleEffect()
{
  deallocateWaveMap(m_previousMap);
  deallocateWaveMap(m_currentMap);
}

qint8 GraphicsRippleEffect::offset() const
{
  return m_offset;
}

qint8 GraphicsRippleEffect::damping() const
{
  return m_damping;
}

qint8 GraphicsRippleEffect::heigth() const
{
  return m_heigth;
}

qreal GraphicsRippleEffect::opacity() const
{
  return m_opacity;
}

void GraphicsRippleEffect::setOffset(qint8 offset)
{
  m_offset = offset;
  emit offsetChanged(m_offset);
}

void GraphicsRippleEffect::setDamping(qint8 damping)
{
  m_damping = damping;
  emit dampingChanged(m_damping);
}

void GraphicsRippleEffect::setHeigth(qint8 heigth)
{
  m_heigth = heigth;
  emit heigthChanged(m_heigth);
}

void GraphicsRippleEffect::setOpacity(qreal opacity)
{
  m_opacity = opacity;
  update();
}

void GraphicsRippleEffect::draw(QPainter *painter)
{
  DOUT("enter");
  QPoint offset;
  const QImage currentImage = sourcePixmap(Qt::LogicalCoordinates, &offset).toImage();
  QImage modifiedImage = currentImage;
  if (!m_previousMap && !m_currentMap) {
    m_previousMap = allocateWaveMap(currentImage.size());
    m_currentMap = allocateWaveMap(currentImage.size());
  }

  int x, y;
  if (qFuzzyCompare(m_opacity, qreal(0.0))) {
    for (x = 0; x < currentImage.width(); ++x) {
      memset(m_currentMap[x], 0, sizeof(int) * currentImage.height());
      memset(m_previousMap[x], 0, sizeof(int) * currentImage.height());
    }
    m_mapSize = currentImage.size();
    int waveLength = m_mapSize.width() > m_mapSize.height() ? m_mapSize.width() : m_mapSize.height();
    m_currentMap[m_mapSize.width() >> 1][m_mapSize.height() >> 1] = waveLength << m_heigth;
  } else if (m_mapSize != currentImage.size()) {
    const qreal scaleFactorX = qreal(currentImage.width()) / qreal(m_mapSize.width());
    const qreal scaleFactorY = qreal(currentImage.height()) / qreal(m_mapSize.height());
    int **newPreviousMap = allocateWaveMap(currentImage.size());
    int **newCurrentMap = allocateWaveMap(currentImage.size());
    int i, j;
    for (y = 0; y < currentImage.height(); ++y) {
      for (x = 0; x < currentImage.width(); ++x) {
        i = x / scaleFactorX;
        j = y / scaleFactorY;
        newPreviousMap[x][y] = m_previousMap[i][j];
        newCurrentMap[x][y] = m_currentMap[i][j];
      }
    }
    deallocateWaveMap(m_previousMap);
    deallocateWaveMap(m_currentMap);
    m_mapSize = currentImage.size();
    m_previousMap = newPreviousMap;
    m_currentMap = newCurrentMap;
  }

  const int width = m_mapSize.width();
  const int height = m_mapSize.height();
  int neighbours;
  int wave;
  int xOffset, yOffset;
  for (y = m_offset; y < height - m_offset - 1; ++y) {
    for (x = m_offset; x < width - m_offset - 1; ++x) {
      neighbours = m_previousMap[x+m_offset][y] +
             m_previousMap[x-m_offset][y] +
             m_previousMap[x][y+m_offset] +
             m_previousMap[x][y-m_offset];
      if (!neighbours && !m_currentMap[x][y]) {
        continue;
      }

      wave = (neighbours >> 1) - m_currentMap[x][y];
      wave -= wave >> m_damping;
      m_currentMap[x][y] = wave;

      xOffset = x + m_currentMap[x+m_offset][y] - wave;
      yOffset = y + m_currentMap[x][y+m_offset] - wave;

      modifiedImage.setPixel(x, y, currentImage.pixel(
            qBound(0, xOffset, width - 1),
            qBound(0, yOffset, height - 1)));
    }
  }

  // Swap wave maps
  int **pointer = m_previousMap;
  m_previousMap = m_currentMap;
  m_currentMap = pointer;

  // Restart wave if image center has no wave
  if (m_currentMap[width >> 1][height >> 1] == 0) {
    int waveLength = width > height ? width : height;
    m_currentMap[width >> 1][height >> 1] = waveLength << m_heigth;
  }

  painter->drawImage(offset, currentImage);
  painter->setOpacity(1 - m_opacity);
  painter->drawImage(offset, modifiedImage);
  DOUT("exit");
}

// EOF
