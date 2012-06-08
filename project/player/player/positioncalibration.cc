// positioncalibration.cc
// 4/8/2012

#include "positioncalibration.h"
#include "signalhub.h"
#include "module/player/player.h"
#include "module/qtext/datetime.h"
#include <QtGui>

//#define DEBUG "positioncalibration"
#include "module/debug/debug.h"

enum { CanvasHeight = 11, // Exactly the same as LabelHeight
       MarginLeft = 10, MarginTop = 0 };

// - Construction -

PositionCalibration::PositionCalibration(SignalHub *hub, Player *player, QWidget *parent)
  : Base(parent), hub_(hub), player_(player)
{
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(0);

}

// - Properties -

bool
PositionCalibration::needsDisplay() const
{ return hub_->isMediaTokenMode() && player_->hasMedia(); }

// - Events -

void
PositionCalibration::updateVisible()
{
  bool t = needsDisplay();
  if (t != isVisible())
    setVisible(t);
}

void
PositionCalibration::setVisible(bool visible)
{
  bool t = visible && needsDisplay();
  if (t != isVisible()) {
    setFixedHeight(t ? CanvasHeight : 0);
    Base::setVisible(t);
    emit visibleChanged(isVisible());
  }
  Base::setVisible(visible);
}

void
PositionCalibration::paintEvent(QPaintEvent *event)
{
  DOUT("enter");
  Q_UNUSED(event);
  if (needsDisplay()) {
    QRect view(MarginLeft, MarginTop,
               width() - MarginLeft, height() - MarginTop);
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing );
    paintCoordinate(painter, view);
  }
  //Base::paintEvent(event);
  DOUT("exit");
}

// - Paint -

void
PositionCalibration::paintCoordinate(QPainter &painter, const QRect &view)
{
  Q_ASSERT(player_->hasMedia());
  enum { FontAlpha = quint8(255 * 0.9) };

  enum { MarginSize = 3 };
#ifdef Q_WS_WIN
  enum { LabelFontSize = 9 };
#else
  enum { LabelFontSize = 10 };
#endif // Q_WS_WIN
  enum { LabelHeight = LabelFontSize + 2 };

  enum { unit = 3000 }; // 3 sec

  qint64 duration = player_->mediaLength();
  if (duration <= 0)
    return;

  int metricScale = hub_->isFullScreenWindowMode() ? 1 : 2;
  int metric = (duration / (1800 * 1000) +1) * unit * metricScale; // unit / 30min
  Q_ASSERT(metric);
  //if (!metric)
  //  metric = 2 * 5*1000; // 10 seconds

  enum { Stride = 90 * 1000 / unit }; // every 1.5 min for metric = unit

  //enum { TopYCount = 3 };
  //int topY[TopYCount] = { };


  int rangeX = duration / metric;
  if (rangeX <= 0)
    return;

  Q_ASSERT(rangeX);

  // Draw histogram
  int width = view.width(),
      height = view.height();

  // Draw label
  {
    QFont f = painter.font();
    f.setPointSize(LabelFontSize);
    f.setBold(true);
    painter.setFont(f);

    QColor color(Qt::cyan);
    color.setAlpha(FontAlpha);

    painter.setPen(color);
    int labelCount = rangeX / Stride + 1;
    Q_ASSERT(labelCount);
    int labelWidth = width / labelCount;
    for (int i = 0; i < labelCount; i++) {
      qint64 msecs = i * Stride * metric;
      QTime t = QtExt::msecs2time(msecs);
      QString label = msecs >= 3600 * 1000 ? t.toString("h:mm:ss")
                                           : t.toString("m:ss");
      int x = MarginSize + i * labelWidth,
          y = height - LabelHeight;
      painter.drawText(QRect(view.x() + x, view.y() + y, labelWidth, LabelHeight), label);
    }
  }
}
// EOF

/*
void
PositionCalibration::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
    emit clicked();
  else
    Base::mouseDoubleClickEvent(event);
}
*/
