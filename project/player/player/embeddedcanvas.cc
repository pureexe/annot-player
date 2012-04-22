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

enum { CanvasHeight = 160,
       MarginLeft = 10, MarginTop = 10 };

// - Construction -

EmbeddedCanvas::EmbeddedCanvas(DataManager *data, SignalHub *hub, Player *player, QWidget *parent)
  : Base(parent), enabled_(true), data_(data), hub_(hub), player_(player), offset_(0)
{
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(0);
}

// - Properties -

bool
EmbeddedCanvas::isEmpty() const
{ return !data_->hasAnnotations(); }

void
EmbeddedCanvas::setEnabled(bool t)
{
  enabled_ = t;
  emit enabledChanged(enabled_);
  setVisible(enabled_);
}

void
EmbeddedCanvas::setOffset(qint64 secs)
{
  if (offset_ != secs) {
    offset_ = secs;
    if (isVisible())
      repaint();
  }
}

// - Events -

void
EmbeddedCanvas::invalidateVisible()
{
  bool t = enabled_ && !isEmpty();
  if (t != isVisible())
    setVisible(t);
}

void
EmbeddedCanvas::setVisible(bool visible)
{
  bool t = enabled_ && visible && !isEmpty();
  if (t != isVisible()) {
    setFixedHeight(t ? CanvasHeight : 0);
    Base::setVisible(t);
    emit visibleChanged(isVisible());
  }
}

void
EmbeddedCanvas::paintEvent(QPaintEvent *event)
{
  DOUT("enter");
  Q_UNUSED(event);
  if (hub_->isMediaTokenMode() && !isEmpty()) {
    QRect view(MarginLeft, MarginTop,
               width() - MarginLeft, height() - MarginTop);
    QPainter painter(this);
    painter.setRenderHints(
      //QPainter::Antialiasing |
      QPainter::TextAntialiasing |
      QPainter::SmoothPixmapTransform
    );
    paintHistogram(painter, view, data_->annotations());
  }
  DOUT("exit");
}

// - Paint -

void
EmbeddedCanvas::paintHistogram(QPainter &painter, const QRect &view, const AnnotationList &l)
{
  if (l.isEmpty())
    return;

  const QString
    tr_peak = tr("peak"),
    tr_average = tr("average"),
    tr_sec = tr("sec.");

  enum { r_start = 255, r_stop = 255,
         g_start = 255, g_stop = 0,
         b_start = 32,  b_stop = 0,
         alpha = quint8(255 * 0.8) };
  enum { FontAlpha = quint8(255 * 0.9) };

  enum { MarginSize = 3 };
#ifdef Q_WS_WIN
  enum { AxisFontSize = 9, LabelFontSize = 9, NoteFontSize = 9, PeakFontSize = 8 };
#else
  enum { AxisFontSize = 10, LabelFontSize = 10, NoteFontSize = 10, PeakFontSize = 9 };
#endif // Q_WS_WIN
  enum { PeakRotation = -20 };
  enum { LabelHeight = LabelFontSize + 2, PeakFontHeight = PeakFontSize + 3 };
  enum { LabelWidth = 100 };

  enum { unit = 3000 }; // 3 sec

  qint64 duration = player_->hasMedia() ? player_->mediaLength() : 0;

  int metricScale = hub_->isFullScreenWindowMode() ? 1 : 2;
  int metric = (duration / (1800 * 1000) +1) * unit * metricScale; // unit / 30min
  Q_ASSERT(metric);
  //if (!metric)
  //  metric = 2 * 5*1000; // 10 seconds

  enum { Stride = 90 * 1000 / unit }; // every 1.5 min for metric = unit

  //enum { TopYCount = 3 };
  //int topY[TopYCount] = { };

  typedef QHash<int, int> Histogram;
  Histogram hist;
  int maxX = 0;
  foreach (const Annotation &a, l) {
    int x = a.pos() / metric;
    if (x < 0)
      continue;
    ++hist[x];
    if (maxX < x)
      maxX = x;
  }
  if (hist.isEmpty())
    return;

  int startX = offset_ * 1000/metric;

  enum { TopCount = 3 };
  int top[TopCount] = { };
  foreach (int y, hist)
    if (top[0] < y) {
      top[2] = top[1];
      top[1] = top[0];
      top[0] = y;
    } else if (top[1] < y) {
      top[2] = top[1];
      top[1] = y;
    } else if (top[2] < y) {
      top[2] = y;
    }

  int maxY = qMax(top[0], 1);

  if (maxX < Stride)
    maxX = Stride;

  int rangeX = duration / metric;
  if (rangeX <= 0)
    rangeX = maxX;

  DOUT("maxX =" << maxX << ", rangeX =" << rangeX << ", maxY =" << maxY);

  Q_ASSERT(maxX);
  Q_ASSERT(rangeX);
  Q_ASSERT(maxY);

  // Draw histogram
  int width = view.width(),
      height = view.height();
  int histHeight = height - (MarginSize + LabelHeight);
  int lineWidth = hub_->isFullScreenWindowMode() && hist.size() < 250 ? 2
                : 1;

  int x0 = width * startX / rangeX;

  for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
    int x = i.key(),
        y = i.value();

    qreal px =  x / (qreal)rangeX,
          py = y / (qreal)maxY;
    x = width * px + x0;
    y = histHeight * py;

    if (x0 && (x < 0 || x > width))
      continue;

    int r = r_start + (r_stop - r_start) * py,
        g = g_start + (g_stop - g_start) * py,
        b = b_start + (b_stop - b_start) * py;
    QColor color(r, g, b, alpha);

    painter.setPen(QPen(color, lineWidth));
    painter.drawLine(view.x() + x, view.y() + histHeight,
                     view.x() + x, view.y() + histHeight - y);
  }

  // Draw sublines
  if (x0) {
    int xx = x0 > 0 && x0 < width ? x0 :
             x0 < 0 && x0 > -width ? width + x0 :
             0;
    if (xx) {
      painter.setPen(QPen(Qt::green, 1, Qt::DotLine));
      painter.drawLine(view.x() + xx, view.y() + histHeight,
                       view.x() + xx, view.y() + histHeight *4/10);
    }
  }

  // Draw axis
  //int axisLength = rangeX == maxX ? width :
  //                 width * maxX / rangeX;
  int axisLength = width;
  if (axisLength) {
    enum { FontMarginLeft = 10, FontMarginRight = 10 };
    enum { FontHeight = AxisFontSize + 3 };

    QFont f = painter.font();
    f.setPointSize(AxisFontSize);
#ifdef Q_WS_WIN
    f.setBold(false);
#else
    f.setBold(true);
#endif // Q_WS_WIN
    painter.setFont(f);

    QColor axisColor(Qt::green);
    axisColor.setAlpha(alpha);

    QColor fontColor(Qt::green);
    fontColor.setAlpha(FontAlpha);

    // line 1/2
    int y = view.y() + histHeight / 2;
    painter.setPen(QPen(axisColor, 1, Qt::DashLine));
    painter.drawLine(view.x(), y,
                     view.x() + axisLength, y);

    // label 1/2
    qreal value = maxY / 2.0;
    if (value >= 0.01) {
      QString label = QString::number(value, 'f', 1);
      painter.setPen(fontColor);;
      QRect r(view.x() + FontMarginLeft, view.y() + y - FontHeight - MarginTop,
              axisLength - (FontMarginLeft + FontMarginRight), FontHeight);
      painter.drawText(r, Qt::AlignTop | Qt::AlignLeft, label);
      label = "%50";
      painter.drawText(r, Qt::AlignTop | Qt::AlignRight, label);
    }

    // line 1/4
    y = view.y() + histHeight * 3 / 4;
    painter.setPen(QPen(axisColor, 1, Qt::DotLine));
    painter.drawLine(view.x(), y,
                     view.x() + axisLength, y);
    // label 1/4
    value = maxY / 4.0;
    if (value >= 0.01) {
      QString label = QString::number(value, 'f', 1);
      painter.setPen(fontColor);;
      QRect r(view.x() + FontMarginLeft, view.y() + y - FontHeight - MarginTop,
              axisLength - (FontMarginLeft + FontMarginRight), FontHeight);
      painter.drawText(r, Qt::AlignTop | Qt::AlignLeft, label);
      label = "%25";
      painter.drawText(r, Qt::AlignTop | Qt::AlignRight, label);
    }
  }

  // Calculate local peaks
  QList<QPoint> peaks;
  {
    enum { MinPeakCount = 6 };
    enum { MaxTries = 3 };
    enum { distanceX = 30 }; // 3sec * 30 = 1.5min
    int t = maxY;
    for (int i = TopCount -1; i>=0; i--)
      if (top[i]) {
        t = top[i];
        break;
      }
    Q_ASSERT(t);
    int deltaY = qMax(t / 10, 1);
    int minY = t / 2 + 2;
    Q_ASSERT(minY >= deltaY);
    int limitY = qMax(t * 5/8, minY);
    while (peaks.size() < MinPeakCount && limitY && limitY >= minY) {
      peaks.clear();
      for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
        int x = i.key(),
            y = i.value();
        if (y < limitY)
          continue;
        if (!peaks.isEmpty() &&
            peaks.last().x() + distanceX > x) {
          if (peaks.last().y() < y)
            peaks.last() = QPoint(x, y);
        } else
          peaks.append(QPoint(x, y));
      }
      if (!deltaY)
        break;
      limitY -= deltaY;
    }
  }

  // Draw peaks
  if (!peaks.isEmpty()) {
    QFont f = painter.font();
    f.setPointSize(PeakFontSize);
    painter.setFont(f);

    foreach (const QPoint &peak, peaks) {
      qreal px = peak.x() / (qreal)rangeX,
            py = peak.y() / (qreal)maxY;
      int x = width * px + x0,
          y = histHeight * py;

      if (x0 && (x < 0 || x > width))
        continue;
      x = view.x() + x,
      y = view.y() + histHeight - y;

      qint64 msecs = peak.x() * metric;
      QTime t = QtExt::msecs2time(msecs);
      QString pos = msecs >= 3600 * 1000 ? t.toString("h:mm:ss")
                                         : t.toString("m:ss");
      QString value = QString("<%1%,").arg(QString::number(peak.y()*100/maxY));

      if (peak.y() >= maxY * 0.9) {
        //pos += QString("(%1)").arg(tr("peak"));
        QRect r(x+1, y-5, x+1 + LabelWidth, y-5 + PeakFontHeight),
              rr;
        painter.setPen(Qt::red);
        QString label = value;
        painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, label, &rr);

        painter.setPen(Qt::cyan);
        label = pos;
        r.setX(rr.right());
        painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, label);
      } else {
        painter.save();
        painter.translate(x, y);
        painter.rotate(PeakRotation);
        //painter.drawText(0, 0, label);

        QRect r(1, -5, LabelWidth, PeakFontHeight),
              rr;
        painter.setPen(Qt::red);
        QString label = value;
        painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, label, &rr);

        painter.setPen(Qt::cyan);
        label = pos;
        r.setX(rr.right());
        painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, label);
        painter.restore();
      }
    }
  }

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

  // Draw statistics at the side
  {
    enum { MarginLeft = 10, MarginRight = 10 };
    //int marginTop = histHeight / (hub_->isFullScreenWindowMode() ? 16 : 5);
    int marginTop = histHeight / (4 + 1);
    int metricInSec = metric / 1000;
    int durationInSec = qMax(1,
      duration ? int(duration / 1000)
               : rangeX * metricInSec
    );
    QString peak = QString::number(maxY),
            averagePerSecond = QString::number(l.size()/qreal(durationInSec), 'f', 2);
    QString note = QString("%1:%5/%6%2  %3:%7/%4")
                   .arg(tr_peak).arg(tr_sec).arg(tr_average).arg(tr_sec)
                   .arg(peak) // %5
                   .arg(QString::number(metricInSec)) // %6
                   .arg(averagePerSecond); // %7

    QFont f = painter.font();
    //f.setItalic(true);
#ifdef Q_WS_WIN
    f.setBold(false);
#else
    f.setBold(true);
#endif // Q_WS_WIN
    f.setPointSize(NoteFontSize);
    painter.setFont(f);
    painter.setPen(Qt::red);
    painter.drawText(QRect(view.x() + MarginLeft, view.y() + marginTop, width, height), note);
    //QPainterPath path;
    //path.addText(MarginLeft, MarginTop, f, note);
    //painter.setPen(color);
    //painter.setBrush(Qt::black);
    //painter.drawPath(path);
  }
}

// EOF

/*
void
EmbeddedCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_ASSERT(event);
  if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
    emit clicked();
  else
    Base::mouseDoubleClickEvent(event);
}
*/
