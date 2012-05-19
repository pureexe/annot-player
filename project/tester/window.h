#ifndef WINDOW_H
#define WINDOW_H

// mainwindow.h
// 5/6/2012

//#include "module/graphicseffect/graphicsrippleeffect.h"
#include <QtGlobal>
//#ifdef Q_WS_WIN
//#  include "win/dwm/dwm.h"
//#endif // Q_WS_WIN
#include <QtGui>
#ifdef Q_WS_MAC
#  include <QtGui/QMacCocoaViewContainer>
#endif // Q_WS_MAC

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

typedef QWidget WindowBase;
class Window : public WindowBase
{
  Q_OBJECT
  Q_DISABLE_COPY(Window)
  typedef Window Self;
  typedef WindowBase Base;

  int **m_previousMap; // Last ripple matrix wave
  int **m_currentMap;  // Current ripple matrix wave
  qreal m_opacity;
  qint8 m_damping;
  qint8 m_heigth;
  qint8 m_offset;
  QSize m_mapSize;
  QPoint center_;
  bool pressed_;
public:
  Window()
    : m_previousMap(0), m_currentMap(0), m_opacity(0.2), m_damping(4), m_heigth(2), m_offset(1),
      pressed_(false), winId_(0)
  {
//#ifdef Q_WS_WIN
//    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_NoSystemBackground);
//    Dwm::enableBlurBehindWindow(this);
//    Dwm::extendFrameIntoClientArea(this);
//#endif // Q_WS_WIN
    setMouseTracking(true);
    resetCenter();

    //GraphicsRippleEffect *e = new GraphicsRippleEffect;
    //e->setDamping(m_damping);
    //e->setHeigth(m_heigth);
    //e->setOpacity(m_opacity);
    //setGraphicsEffect(e);

    resize(300,200);
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SLOT(repaint()));
    t->start(20);
  }
  WId winId_;

public slots:
  void setVisible(bool visible)
  {
    //qDebug() << "window::setVisible:enter: visible =" << visible;
    Base::setVisible(visible);
    //qDebug() << "window::setVisible:exit";
  }

  void setCenter(const QPoint &pos)
  { center_ = pos; }

  void setCenter(int x, int y)
  { setCenter(QPoint(x, y)); }

  void resetCenter()
  { setCenter(-1, -1); }

protected:
  // See: http://freespace.virgin.net/hugo.elias/graphics/x_water.htm
  void mousePressEvent(QMouseEvent *e)
  {
    pressed_ = true;
    m_damping <<= 2;
    setCenter(mapFromGlobal(e->globalPos()));
    Base::mousePressEvent(e);
  }
  void mouseReleaseEvent(QMouseEvent *e)
  {
    pressed_ = false;
    m_damping >>= 2;
    resetCenter();
    Base::mouseReleaseEvent(e);
  }
  void mouseMoveEvent(QMouseEvent *e)
  {
    if (pressed_)
      setCenter(mapFromGlobal(e->globalPos()));
    Base::mouseMoveEvent(e);
  }

  void paintEvent(QPaintEvent *event)
  {
    if (!winId_)
      return;
    Q_UNUSED(event);
    //qDebug() << "paintEvent:enter";
    QPainter painter(this);
    //QImage currentImage("/Users/jichi/Dropbox/Library/Wallpapers/sample.jpg");
    //QImage currentImage("C:/Users/jichi/Dropbox/Library/Wallpapers/sample.jpg");
    //QImage currentImage(size(), QImage::Format_ARGB32);
    //currentImage.fill(Qt::cyan);
    //Base::render(&currentImage);
    WId hwnd = winId_ ? winId_ : winId();
    QImage currentImage = QPixmap::grabWindow(hwnd).toImage();
    QImage modifiedImage = currentImage;

    int x, y;

    if (!m_previousMap && !m_currentMap) {
      m_previousMap = ::allocateWaveMap(size());
      m_currentMap = ::allocateWaveMap(size());

      for (x = 0; x < width(); ++x) {
        ::memset(m_currentMap[x], 0, sizeof(int) * height());
        ::memset(m_previousMap[x], 0, sizeof(int) * height());
      }
      m_mapSize = size();
      if (rect().contains(center_)) {
        int waveLength = qMax(m_mapSize.width(), m_mapSize.height());
        m_currentMap[center_.x()][center_.y()] = waveLength << m_heigth;
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
    if (m_mapSize != size()) {
      const qreal scaleFactorX = qreal(width()) / qreal(m_mapSize.width());
      const qreal scaleFactorY = qreal(height()) / qreal(m_mapSize.height());
      int **newPreviousMap = allocateWaveMap(size());
      int **newCurrentMap = allocateWaveMap(size());
      int i, j;
      for (y = 0; y < height(); ++y) {
        for (x = 0; x < width(); ++x) {
          i = x / scaleFactorX;
          j = y / scaleFactorY;
          newPreviousMap[x][y] = m_previousMap[i][j];
          newCurrentMap[x][y] = m_currentMap[i][j];
        }
      }
      deallocateWaveMap(m_previousMap);
      deallocateWaveMap(m_currentMap);
      m_mapSize = size();
      m_previousMap = newPreviousMap;
      m_currentMap = newCurrentMap;
    }

    const int width = m_mapSize.width();
    const int height = m_mapSize.height();
    for (y = m_offset; y < height - m_offset - 1; ++y) {
      for (x = m_offset; x < width - m_offset - 1; ++x) {
        int neighbours = m_previousMap[x+m_offset][y] +
               m_previousMap[x-m_offset][y] +
               m_previousMap[x][y+m_offset] +
               m_previousMap[x][y-m_offset];
        if (!neighbours && !m_currentMap[x][y]) {
          continue;
        }

        int wave = (neighbours >> 1) - m_currentMap[x][y];
        wave -= wave >> m_damping;
        m_currentMap[x][y] = wave;

        int xOffset = x + m_currentMap[x+m_offset][y] - wave,
            yOffset = y + m_currentMap[x][y+m_offset] - wave;

        modifiedImage.setPixel(x, y, currentImage.pixel(
              qBound(0, xOffset, width - 1),
              qBound(0, yOffset, height - 1)));
      }
    }
    // Swap wave maps
    qSwap(m_previousMap, m_currentMap);

    // Restart wave if image center has no wave
    if (!m_mapSize.isEmpty() && rect().contains(center_)) { // && m_currentMap[width >> 1][height >> 1] == 0) {
      int waveLength = width > height ? width : height;
      m_currentMap[center_.x()][center_.y()] = waveLength << m_heigth;
    }

    QPoint offset(0, 0);
    painter.drawImage(offset, currentImage);
    painter.setOpacity(1 - m_opacity);
    painter.drawImage(offset, modifiedImage);
    //qDebug() << "paintEvent:exit";
  }
};

#endif // WINDOW_H
