// rippleimagefilter.cc
// 5/11/2012
#include "module/imagefilter/rippleimagefilter.h"
#include <QtGui/QPixmap>
#include <QtGui/QImage>

// - Construction -

namespace { // anonymous
  inline int **allocateWaveMap(const QSize &size)
  {
    int **waveMap = new int *[size.width()];
    if (!waveMap) {
      return 0;
    }

    for (int x = 0; x < size.width(); ++x) {
      waveMap[x] = new int[size.height()];
      if (!waveMap[x]) {
        return 0;
      }
    }

    return waveMap;
  }

  inline void deallocateWaveMap(int **waveMap)
  {
    if (waveMap) {
      delete[] *waveMap;
      delete[] waveMap;
    }
  }
} // anymous namespace

RippleImageFilter::RippleImageFilter(QObject *parent)
  : Base(parent),
    previousMap_(0), currentMap_(0), dirty_(false),
    opacity_(0.2), damping_(4), heigth_(2), offset_(1)
{
  clearCenter();
}

RippleImageFilter::~RippleImageFilter()
{
  ::deallocateWaveMap(previousMap_);
  ::deallocateWaveMap(currentMap_);
}

void
RippleImageFilter::clear()
{
  clearCenter();
  ::deallocateWaveMap(previousMap_);
  ::deallocateWaveMap(currentMap_);

  previousMap_ = currentMap_ = 0;
  mapSize_ = QSize();
  dirty_ = false;
}

// - Paint -

void
RippleImageFilter::drawImage(QPainter &painter, const QPointF &pos, const QImage &currentImage) const
{
  if (currentImage.isNull())
    return;
  if (!needsDisplay()) {
    painter.drawImage(pos, currentImage);
    return;
  }
  QSize currentSize = currentImage.size();
  if (currentSize.isEmpty() ||
      currentSize.width() <= 2 * offset_ || currentSize.height() <= 2 * offset_) {
    painter.drawImage(pos, currentImage);
    return;
  }

  dirty_ = false;
  QImage modifiedImage = currentImage;

  // Init currentImage

  int x, y;
  if (!previousMap_ && !currentMap_) {
    previousMap_ = ::allocateWaveMap(currentSize);
    currentMap_ = ::allocateWaveMap(currentSize);

    for (x = 0; x < currentSize.width(); ++x) {
      ::memset(currentMap_[x], 0, sizeof(int) * currentSize.height());
      ::memset(previousMap_[x], 0, sizeof(int) * currentSize.height());
    }
    mapSize_ = currentSize;
    if (hasCenter() &&
        QRect(QPoint(), currentSize).contains(center_)) {
      int waveLength = qMax(mapSize_.width(), mapSize_.height());
      currentMap_[center_.x()][center_.y()] = waveLength << heigth_;
    }
  }

  //if (qFuzzyCompare(m_opacity +1, 1)) {
  //  for (x = 0; x < width(); ++x) {
  //    ::memset(m_currentMap[x], 0, sizeof(int) * height());
  //    ::memset(m_previousMap[x], 0, sizeof(int) * height());
  //  }
  //  m_mapSize = size();
  //  int waveLength = m_mapSize.width() > m_mapSize.height() ? m_mapSize.width() : m_mapSize.height();
  //  m_currentMap[m_mapSize.width() >> 1][m_mapSize.height() >> 1] = waveLength << m_heigth;
  //}
  Q_ASSERT(!mapSize_.isEmpty());
  if (mapSize_ != currentSize) {
    const qreal scaleFactorX = currentSize.width() / qreal(mapSize_.width());
    const qreal scaleFactorY = currentSize.height() / qreal(mapSize_.height());
    int **newPreviousMap = ::allocateWaveMap(currentSize);
    int **newCurrentMap = ::allocateWaveMap(currentSize);
    int i, j;
    for (y = 0; y < currentSize.height(); ++y) {
      for (x = 0; x < currentSize.width(); ++x) {
        i = x / scaleFactorX;
        j = y / scaleFactorY;
        newPreviousMap[x][y] = previousMap_[i][j];
        newCurrentMap[x][y] = currentMap_[i][j];
      }
    }
    ::deallocateWaveMap(previousMap_);
    ::deallocateWaveMap(currentMap_);
    mapSize_ = currentSize;
    previousMap_ = newPreviousMap;
    currentMap_ = newCurrentMap;
  }

  // Process modifiedImage

  const int width = mapSize_.width();
  const int height = mapSize_.height();
  int neighbours, wave, xOffset, yOffset;
  for (y = offset_; y < height - offset_ - 1; y++) {
    for (x = offset_; x < width - offset_ - 1; x++) {
      //int
      neighbours = previousMap_[x+offset_][y] +
                   previousMap_[x-offset_][y] +
                   previousMap_[x][y+offset_] +
                   previousMap_[x][y-offset_];
      if (!neighbours && !currentMap_[x][y])
        continue;
      dirty_ = true;

      //int
      wave = (neighbours >> 1) - currentMap_[x][y];
      wave -= wave >> damping_;
      currentMap_[x][y] = wave;

      //int
      xOffset = x + currentMap_[x+offset_][y] - wave,
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
  if (!mapSize_.isEmpty() && hasCenter() &&
      QRect(QPoint(), mapSize_).contains(center_)) { // && m_currentMap[width >> 1][height >> 1] == 0) {
    int waveLength = width > height ? width : height;
    currentMap_[center_.x()][center_.y()] = waveLength << heigth_;
  }

  painter.drawImage(pos, currentImage);
  painter.setOpacity(1 - opacity_);
  painter.drawImage(pos, modifiedImage);
  //qDebug() << "paintEvent:exit";
}

// EOF
