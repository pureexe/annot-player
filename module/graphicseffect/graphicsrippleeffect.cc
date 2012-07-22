// graphicsrippleeffect.cc
// 5/3/2012

#include "module/graphicseffect/graphicsrippleeffect.h"
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

#define DEBUG "graphicsrippleeffect"
#include "module/debug/debug.h"

// - Construction -

namespace { namespace detail{

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
      delete[] *waveMap;
      delete[] waveMap;
    }
  }

} } // anonymous detail

GraphicsRippleEffect::GraphicsRippleEffect(QObject *parent)
  : QGraphicsEffect(parent),
    offset_(1),
    damping_(16),
    heigth_(1),
    opacity_(0.0),
    mapSize_(boundingRect().size().toSize()),
    previousMap_(nullptr),
    currentMap_(nullptr)
{ }

GraphicsRippleEffect::~GraphicsRippleEffect()
{
  detail::deallocateWaveMap(previousMap_);
  detail::deallocateWaveMap(currentMap_);
}

// - Properties -

void
GraphicsRippleEffect::setOpacity(qreal opacity)
{
  if (!qFuzzyCompare(opacity_, opacity)) {
    opacity_ = opacity;
    update();
  }
}

// - Paint -

void
GraphicsRippleEffect::draw(QPainter *painter)
{
  DOUT("enter");
  QPoint offset;
  const QImage currentImage = sourcePixmap(Qt::LogicalCoordinates, &offset).toImage();
  QImage modifiedImage = currentImage;
  if (!previousMap_ && !currentMap_) {
    previousMap_ = detail::allocateWaveMap(currentImage.size());
    currentMap_ = detail::allocateWaveMap(currentImage.size());
  }

  //DOUT("current image size =" << currentImage.size());
  int x, y;
  if (qFuzzyCompare(opacity_ +1, 1)) {
    for (x = 0; x < currentImage.width(); ++x) {
      qMemSet(currentMap_[x], 0, sizeof(int) * currentImage.height());
      qMemSet(previousMap_[x], 0, sizeof(int) * currentImage.height());
    }
    mapSize_ = currentImage.size();
    if (QRect(QPoint(), mapSize_).contains(center_)) {
      int waveLength = mapSize_.width() > mapSize_.height() ? mapSize_.width() : mapSize_.height();
      currentMap_[center_.x()][center_.y()] = waveLength << heigth_;
    }
  } else if (mapSize_ != currentImage.size()) {
    const qreal scaleFactorX = qreal(currentImage.width()) / qreal(mapSize_.width());
    const qreal scaleFactorY = qreal(currentImage.height()) / qreal(mapSize_.height());
    int **newPreviousMap = detail::allocateWaveMap(currentImage.size());
    int **newCurrentMap = detail::allocateWaveMap(currentImage.size());
    int i, j;
    for (y = 0; y < currentImage.height(); ++y) {
      for (x = 0; x < currentImage.width(); ++x) {
        i = x / scaleFactorX;
        j = y / scaleFactorY;
        newPreviousMap[x][y] = previousMap_[i][j];
        newCurrentMap[x][y] = currentMap_[i][j];
      }
    }
    detail::deallocateWaveMap(previousMap_);
    detail::deallocateWaveMap(currentMap_);
    mapSize_ = currentImage.size();
    previousMap_ = newPreviousMap;
    currentMap_ = newCurrentMap;
  }

  const int width = mapSize_.width();
  const int height = mapSize_.height();
  int neighbours;
  int wave;
  int xOffset, yOffset;
  for (y = offset_; y < height - offset_ - 1; ++y) {
    for (x = offset_; x < width - offset_ - 1; ++x) {
      neighbours = previousMap_[x+offset_][y] +
                   previousMap_[x-offset_][y] +
                   previousMap_[x][y+offset_] +
                   previousMap_[x][y-offset_];
      if (!neighbours && !currentMap_[x][y])
        continue;

      wave = (neighbours >> 1) - currentMap_[x][y];
      wave -= wave >> damping_;
      currentMap_[x][y] = wave;

      xOffset = x + currentMap_[x+offset_][y] - wave;
      yOffset = y + currentMap_[x][y+offset_] - wave;

      modifiedImage.setPixel(x, y, currentImage.pixel(
        qBound(0, xOffset, width - 1),
        qBound(0, yOffset, height - 1)
      ));
    }
  }
  // Swap wave maps
  qSwap(previousMap_, currentMap_);

  // Restart wave if image center has no wave
  if (!mapSize_.isEmpty() &&
      QRect(QPoint(), mapSize_).contains(center_)) {
    int waveLength = width > height ? width : height;
    currentMap_[center_.x()][center_.y()] = waveLength << heigth_;
  }

  painter->drawImage(offset, currentImage);
  painter->setOpacity(1 - opacity_);
  painter->drawImage(offset, modifiedImage);
  DOUT("exit");
}

// EOF
