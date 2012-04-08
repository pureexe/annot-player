// embeddedcanvas.cc
// 4/8/2012

#include "embeddedcanvas.h"
#include "datamanager.h"
#include "signalhub.h"
#include "module/player/player.h"
#include "module/qtext/datetime.h"
#include <QtGui>

//#define DEBUG "embeddedcanvas"
#include "module/debug/debug.h"

using namespace AnnotCloud;

enum { CanvasHeight = 150 };

// - Construction -

EmbeddedCanvas::EmbeddedCanvas(DataManager *data, SignalHub *hub, Player *player, QWidget *parent)
  : Base(parent), data_(data), hub_(hub), player_(player)
{
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  setFixedHeight(CanvasHeight);
}

// - Events -

void
EmbeddedCanvas::paintEvent(QPaintEvent *event)
{
  DOUT("enter");
  Q_UNUSED(event);
  if (hub_->isMediaTokenMode() && data_->hasAnnotations()) {
    QPainter painter(this);
    paint(painter, data_->annotations());
  }
  DOUT("exit");
}

// - Paint -

void
EmbeddedCanvas::paint(QPainter &painter, const AnnotationList &l)
{
  enum { metric = 10 * 1000 }; // every 10 seconds
  enum { alpha = quint8(255 * 0.8),
         r_start = 255, r_stop = 255,
         g_start = 255, g_stop = 0,
         b_start = 32,  b_stop = 0 };

  enum { MarginSize = 3, LabelHeight = 12 };
  enum { Stride = 5 * 6 }; // every 5 min

  enum { LabelFontSize = 10, NoteFontSize = 10 }; // 10em

  if (l.isEmpty())
    return;

  typedef QHash<int, int> Histogram;
  Histogram hist;
  int maxX = 0, maxY = 0;
  foreach (const Annotation &a, l) {
    int x = a.pos() / metric;
    if (x < 0)
      continue;
    int y = ++hist[x];
    if (maxX < x)
      maxX = x;
    if (maxY < y)
      maxY = y;
  }

  if (maxX < Stride)
    maxX = Stride;

  qint64 duration = player_->hasMedia() ? player_->mediaLength() : 0;
  int rangeX = duration / metric;
  if (!rangeX)
    rangeX = maxX;

  DOUT("maxX =" << maxX << ", rangeX =" << rangeX << ", maxY =" << maxY);

  Q_ASSERT(rangeX);
  Q_ASSERT(maxY);

  // Draw histogram
  int width = painter.device()->width(),
      height = painter.device()->height();
  int histHeight = height - (MarginSize + LabelHeight);
  int lineWidth = hub_->isFullScreenWindowMode()  ||
                  rangeX > 3600 * 1000 / metric ? 2
                                                 : 1;
  for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
    int x = i.key(),
        y = i.value();

    qreal px = x / (qreal)rangeX,
          py = y / (qreal)maxY;
    x = width * px;
    y = histHeight * py;

    int r = r_start + (r_stop - r_start) * py,
        g = g_start + (g_stop - g_start) * py,
        b = b_start + (b_stop - b_start) * py;
    QColor color(r, g, b, alpha);

    painter.setPen(QPen(color, lineWidth));
    painter.drawLine(x, histHeight, x, histHeight - y);
  }

  // Draw axix label
  {
    QFont f = painter.font();
    f.setPointSize(LabelFontSize);
    f.setBold(true);
    painter.setFont(f);

    painter.setPen(Qt::cyan);
    int labelCount = rangeX / Stride + 1;
    int labelWidth = width / labelCount;
    for (int i = 0; i < labelCount; i++) {
      qint64 msecs = i * Stride * metric;
      QString label = QtExt::msecs2time(msecs).toString();
      int x = MarginSize + i * labelWidth,
          y = height - LabelHeight;
      painter.drawText(QRectF(x, y, labelWidth, LabelHeight), label);
    }
  }

  // Draw statistics at the side
  {
    enum { MarginTop = 10, MarginLeft = 10 };
    QString peak = QString::number(maxY),
            average = QString().sprintf("%.2f", l.size() / (qreal)rangeX);
    QString note = QString("peak: %2 / 10secs.  average: %1 / 10secs.")
                   .arg(peak).arg(average);

    QFont f = painter.font();
    //f.setItalic(true);
    f.setBold(true);
    f.setPointSize(NoteFontSize);
    painter.setFont(f);
    painter.setPen(Qt::red);
    painter.drawText(QRectF(MarginLeft, MarginTop, width, height), note);
    //QPainterPath path;
    //path.addText(MarginLeft, MarginTop, f, note);
    //painter.setPen(Qt::red);
    //painter.setBrush(Qt::black);
    //painter.drawPath(path);
  }
}

// EOF
