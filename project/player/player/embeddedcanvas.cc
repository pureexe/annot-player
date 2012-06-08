// embeddedcanvas.cc
// 4/8/2012

#include "embeddedcanvas.h"
#include "datamanager.h"
#include "signalhub.h"
#include "annotationsettings.h"
#include "module/player/player.h"
#include "module/qtext/datetime.h"
#include <QtGui>
#include <boost/foreach.hpp>
#include <list>

//#define DEBUG "embeddedcanvas"
#include "module/debug/debug.h"

using namespace AnnotCloud;

enum { CanvasHeight = 150,
       MarginLeft = 10, MarginTop = 10 };

// - Construction -

EmbeddedCanvas::EmbeddedCanvas(DataManager *data, SignalHub *hub, Player *player, QWidget *parent)
  : Base(parent), enabled_(true), data_(data), hub_(hub), player_(player)
{
  Q_ASSERT(data_);
  Q_ASSERT(hub_);
  setContentsMargins(0, 0, 0, 0);
  setFixedHeight(0);

  connect(player_, SIGNAL(mediaChanged()), SLOT(clearUserIds()));
  connect(AnnotationSettings::globalInstance(), SIGNAL(offsetChanged(int)), SLOT(invalidatePaint()));
}

// - Properties -

bool
EmbeddedCanvas::needsDisplay() const
{ return hub_->isMediaTokenMode() && data_->hasAnnotations() && player_->hasMedia(); }

void
EmbeddedCanvas::setEnabled(bool t)
{
  enabled_ = t;
  emit enabledChanged(enabled_);
  setVisible(enabled_);
}

// - Events -

void
EmbeddedCanvas::updateVisible()
{
  bool t = enabled_ && needsDisplay();
  if (t != isVisible())
    setVisible(t);
}

void
EmbeddedCanvas::setVisible(bool visible)
{
  bool t = enabled_ && visible && needsDisplay();
  if (t != isVisible()) {
    setFixedHeight(t ? CanvasHeight : 0);
    Base::setVisible(t);
    emit visibleChanged(isVisible());
  }
  Base::setVisible(visible);
}

void
EmbeddedCanvas::paintEvent(QPaintEvent *event)
{
  DOUT("enter");
  Q_UNUSED(event);
  if (needsDisplay()) {
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
  //Base::paintEvent(event);
  DOUT("exit");
}

// - Paint -

void
EmbeddedCanvas::drawCross(QPainter &painter, const QPoint &center, int size)
{
  int x1, y1, x2, y2; {
    QRect r(0, 0, size, size);
    r.moveCenter(center);
    r.getCoords(&x1, &y1, &x2, &y2);
  }

  painter.drawLine(x1, y1, x2, y2);
  painter.drawLine(x1, y2, x2, y1);
}

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

  //qint64 duration = player_->hasMedia() ? player_->mediaLength() : 0;
  Q_ASSERT(player_->hasMedia());
  qint64 duration = player_->mediaLength();

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

  int offset = AnnotationSettings::globalInstance()->offset(); // in seconds
  int startX = offset * 1000/metric;

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
  Q_ASSERT(maxY);

  int relativeY = top[2] ? top[2] :
                  top[1] ? top[1] :
                  top[0] ? top[0] : maxY;
  Q_ASSERT(relativeY);

  if (maxX < Stride)
    maxX = Stride;

  int rangeX = duration / metric;
  if (rangeX <= 0)
    rangeX = maxX;

  DOUT("maxX =" << maxX << ", rangeX =" << rangeX << ", maxY =" << maxY);

  Q_ASSERT(maxX);
  Q_ASSERT(rangeX);

  // Draw histogram
  int width = view.width(),
      height = view.height();
  int histHeight = height - MarginSize; // - LabelHeight;
  int lineWidth = hub_->isFullScreenWindowMode() && hist.size() < 250 ? 2
                : 1;

  int x0 = width * startX / rangeX;

  for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
    int x = i.key(),
        y = i.value();

    qreal px =  x / qreal(rangeX),
          py = y / qreal(maxY);
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

    QColor fontColor(Qt::cyan);
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
      //label = "%" + QString::number(50 * maxY/relativeY);
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
      //label = "%" + QString::number(25 * maxY/relativeY);
      label = "%25";
      painter.drawText(r, Qt::AlignTop | Qt::AlignRight, label);
    }
  }

  // Calculate local peaks
  std::list<QPoint> peaks; // use list instead of QList to improve performance
  {
    enum { MinPeakCount = 6 };
    enum { MaxTries = 3 };
    enum { distanceX = 30 }; // 3sec * 30 = 1.5min
    int deltaY = qMax(relativeY / 10, 1);
    int minY = relativeY / 2 + 2;
    Q_ASSERT(minY >= deltaY);
    int limitY = qMax(relativeY * 5/8, minY);
    while (peaks.size() < MinPeakCount && limitY && limitY >= minY) {
      peaks.clear();
      for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
        int x = i.key(),
            y = i.value();
        if (y < limitY)
          continue;
        if (!peaks.empty() &&
            peaks.back().x() + distanceX > x) {
          if (peaks.back().y() < y)
            peaks.back() = QPoint(x, y);
        } else
          peaks.push_back(QPoint(x, y));
      }
      if (!deltaY)
        break;
      limitY -= deltaY;
    }
  }

  // Draw peaks
  if (!peaks.empty()) {
    QFont f = painter.font();
    f.setPointSize(PeakFontSize);
    painter.setFont(f);

    BOOST_FOREACH (const QPoint &peak, peaks) {
      qreal px = peak.x() / qreal(rangeX),
            py = peak.y() / qreal(maxY);
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
  /*
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
  */

  // Calculate highlighted positions.
  std::list<qint64> highlights;
  if (!userIds_.isEmpty())
    foreach (const Annotation &a, data_->annotations())
      if (userIds_.contains(a.userId()))
        highlights.push_back(a.pos());
  DOUT("highlights count =" << highlights.size());

  // Draw highlights
  if (!highlights.empty()) {
    painter.setPen(Qt::cyan);
    int y = view.y() + histHeight * 3 / 4;

    BOOST_FOREACH (qint64 pos, highlights) {
      int x = pos / metric;
      qreal px =  x / qreal(rangeX);
      x = width * px + x0;

      if (x0 && (x < 0 || x > width))
        continue;

      drawCross(painter, QPoint(view.x() + x, y));
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
    if (!userIds_.isEmpty())
      note.append(QString("  %1:%2%3")
        .arg(tr("traced users"))
        .arg(QString::number(userIds_.size()))
        .arg(tr("u"))
      );

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

void
EmbeddedCanvas::paintCoordinate(QPainter &painter, const QRect &view)
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
*/
