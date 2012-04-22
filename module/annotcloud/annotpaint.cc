// annotcloud/annotpaint.cc
// 4/6/2012

#include "module/annotcloud/annotpaint.h"
#include "module/qtext/datetime.h"
#include "module/qtext/algorithm.h"
#ifdef WITH_QT_SVG
#  include <QtSvg/QSvgGenerator>
#endif // WITH_QT_SVG
#include <QtGui/QPainter>
#include <QtCore/QHash>
#include <QtCore/QMultiMap>
#include <QtCore/QDateTime>
//#include <boost/bimap.hpp>
#include <utility>
#include <climits>

#define DEBUG "annotpaint"
#include "module/debug/debug.h"

#ifdef Q_WS_WIN
#  define ITALIC        false
#else
#  define ITALIC        true
#endif Q_WS_WIN

enum LineColor
{
  alpha = quint8(255 * 0.9),
  r_start = 255, r_stop = 255,
  g_start = 255, g_stop = 0,
  b_start = 32,  b_stop = 0
};

void
AnnotationPainter::paintAxis(QPainter &p, const QRect &view, int maxY) const
{
  enum { AxisFontSize = 8 };
  enum { FontHeight = AxisFontSize + 3 };
  enum { FontMarginLeft = 10, FontMarginRight = 10 };

  QFont f = p.font();
  f.setPointSize(AxisFontSize);
  f.setBold(true);
  p.setFont(f);

  QColor color(Qt::green);
  color.setAlpha(alpha);

  // line 1/2
  int y = view.y() + view.height() / 2;
  p.setPen(QPen(color, 1, Qt::DashLine));
  p.drawLine(view.left(), y, view.right(), y);

  // label 1/2
  qreal value = maxY / 2.0;
  if (value >= 0.01) {
    QString label = QString::number(value, 'f', 2);
    p.setPen(color);;
    QRect r(view.x() + FontMarginLeft, view.y() + y - FontHeight,
            view.width() - (FontMarginLeft + FontMarginRight), FontHeight);
    p.drawText(r, Qt::AlignTop | Qt::AlignLeft, label);
    label = "50%";
    p.drawText(r, Qt::AlignTop | Qt::AlignRight, label);
  }

  // line 1/4
  y = view.y() + view.height() * 3 / 4;
  p.setPen(QPen(color, 1, Qt::DotLine));
  p.drawLine(view.left(), y, view.right(), y);
  // label 1/4
  value = maxY / 4.0;
  if (value >= 0.01) {
    QString label = QString::number(value, 'f', 2);
    p.setPen(color);;
    QRect r(view.x() + FontMarginLeft, view.y() + y - FontHeight,
            view.width() - (FontMarginLeft + FontMarginRight), FontHeight);
    p.drawText(r, Qt::AlignTop | Qt::AlignLeft, label);
    label = "25%";
    p.drawText(r, Qt::AlignTop | Qt::AlignRight, label);
  }
}

void
AnnotCloud::
AnnotationPainter::paintHistogram(QPainter &p, const AnnotationList &l, Annotation::Field sortBy, const QString &title, const QRect &view) const
{
  switch (sortBy) {
  case Annotation::Pos: paintHistogramByPos(p, l, title, view); break;
  case Annotation::CreateTime: paintHistogramByCreateTime(p, l, title, view); break;
  case Annotation::UserId: paintHistogramByUserId(p, l, title, view); break;
  default: Q_ASSERT(0); break;
  }
}

void
AnnotCloud::
AnnotationPainter::paintHistogramByPos(QPainter &p, const AnnotationList &l, const QString &title, const QRect &view) const
{
  DOUT("count =" << l.size());
  if (l.isEmpty())
    return;

  enum { LabelFontSize = 10, PeakFontSize = 8 }; // 10em
  enum { LabelHeight = LabelFontSize + 2, PeakFontHeight = PeakFontSize + 2 };
  enum { LabelWidth = 50 };

  enum { MarginSize = 3 };
  enum { Stride = 10 * 6 }; // every 5 min

  enum { unit = 5 * 1000 }; // every 10 seconds

  // Analysis
  qint64 duration = 0;
  foreach (const Annotation &a, l)
    if (duration < a.pos())
      duration = a.pos();

  int metric = (duration / (1800 * 1000) +1) * unit; // unit / 30min
  Q_ASSERT(metric);

  typedef QHash<int, int> Histogram;
  Histogram hist;
  foreach (const Annotation &a, l) {
    int x = a.pos() / metric;
    if (x < 0)
      continue;
    ++hist[x];
  }
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

  DOUT("hash size =" << hist.size());
  DOUT("maxY =" << maxY);

  int maxX = duration / metric;

  if (maxX < Stride)
    maxX = Stride;

  Q_ASSERT(maxX);
  Q_ASSERT(maxY);

  // Draw histogram
  int width = view.width() ? view.width() : p.device()->width(),
      height = view.height() ? view.height() : p.device()->height();
  int histHeight = height - (MarginSize + LabelHeight);
  for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
    int x = i.key(),
        y = i.value();

    qreal px = x / qreal(maxX),
          py = y / qreal(maxY);

    int r = r_start + (r_stop - r_start) * py,
        g = g_start + (g_stop - g_start) * py,
        b = b_start + (b_stop - b_start) * py;
    QColor color(r, g, b, alpha);

    x = width * px;
    y = histHeight * py;

    p.setPen(color);
    p.drawLine(view.x() + x, view.y() + histHeight,
               view.x() + x, view.y() + histHeight - y);
  }

  // Draw axis
  paintAxis(p, QRect(view.x(), view.y(), width, histHeight), maxY);

  // Draw label
  {
    int y = view.y() + height - LabelHeight;
    p.setPen(Qt::cyan);
    QFont f = p.font();
    f.setPointSize(LabelFontSize);
    f.setBold(true);
    p.setFont(f);
    int labelCount = maxX / Stride + 1;
    Q_ASSERT(labelCount);
    int labelWidth = width / labelCount;
    for (int i = 0; i < labelCount; i++) {
      qint64 msecs = i * Stride * metric;
      QTime t = QtExt::msecs2time(msecs);
      QString label = msecs >= 3600 * 1000 ? t.toString("h:mm:ss")
                                           : t.toString("m:ss");
      int x = view.x() + MarginSize + i * labelWidth;
      p.drawText(QRect(x, y, labelWidth, LabelHeight), label);
    }
  }

  // Calculate local peaks
  QList<QPoint> peaks;
  {
    enum { distanceX = 20 }; // 5sec * 20 = 1min 40sec
    int t = maxY;
    for (int i = TopCount -1; i>=0; i--)
      if (top[i]) {
        t = top[i];
        break;
      }
    int limitY = t *5/8 + 1;
    Q_ASSERT(limitY);
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
  }

  DOUT("peaks =" << peaks);

  // Draw peaks
  if (!peaks.isEmpty()) {
    enum { LabelRotation = -20 };
    QFont f = p.font();
    f.setPointSize(PeakFontSize);
    p.setFont(f);
    p.setPen(Qt::green);

    foreach (const QPoint &peak, peaks) {
      qreal px = peak.x() / qreal(maxX),
            py = peak.y() / qreal(maxY);
      int x = width * px,
          y = histHeight * py;

      x = view.x() + x,
      y = view.y() + histHeight - y;

      qint64 msecs = peak.x() * metric;
      QTime t = QtExt::msecs2time(msecs);
      QString pos = msecs >= 3600 * 1000 ? t.toString("h:mm:ss")
                                         : t.toString("m:ss");
      QString label = QString("<%1%,%2")
          .arg(QString::number(peak.y() * 100 / maxY))
          .arg(pos);

      if (peak.y() >= maxY * 0.9)
        p.drawText(QRect(x, y, x + LabelWidth, y + PeakFontHeight), label);
      else {
        p.save();
        p.translate(x, y);
        p.rotate(LabelRotation);
        p.drawText(0, 0, label);
        p.restore();
      }
    }
  }

  // Draw title
  if (!title.isEmpty()) {
    enum { TitleFontSize = 20, NoteFontSize = 10 };
    enum { TitleHeight = TitleFontSize, NoteHeight = NoteFontSize, NoteMargin = 5 };

    int titleHeight = qMin(height, int(TitleHeight));
    {
      QFont f = p.font();
      f.setBold(true);
      f.setPointSize(TitleFontSize);
      p.setFont(f);
      p.setPen(Qt::cyan);
      p.drawText(QRect(0, 0, width, titleHeight), Qt::AlignCenter, title);
    }

    if (height > TitleHeight + NoteHeight + NoteMargin) {
      QString peak = QString::number(maxY),
              averagePerSecond = QString::number(l.size()/(maxX*10.0), 'f', 2),
              unit = QString::number(metric / 1000);

      QString note = QString("peak = %1 / %2 secs.   average = %3 / sec.")
                     .arg(peak).arg(unit).arg(averagePerSecond);

      QFont f = p.font();
      f.setBold(true);
      f.setItalic(ITALIC);
      f.setPointSize(NoteFontSize);
      p.setFont(f);
      p.setPen(QColor("orange"));
      p.drawText(QRect(0, TitleHeight + NoteMargin, width, NoteHeight), Qt::AlignCenter, note);
    }
  }
}

void
AnnotCloud::
AnnotationPainter::paintHistogramByCreateTime(QPainter &p, const AnnotationList &l, const QString &title, const QRect &view) const
{
  DOUT("count =" << l.size());
  if (l.isEmpty())
    return;

  enum { MarginSize = 10, LabelHeight = 30 };
  enum { DateFontSize = 8, HourFontSize = 8 }; // 10em
  enum { PeakFontSize = 8, PeakFontHeight = PeakFontSize + 2 }; // 10em
  enum { LabelWidth = 50 };

  // Analysis
  qint64 max = 0,
         min = LLONG_MAX;
  foreach (const Annotation &a, l)  {
    qint64 t = a.createTime();
    if (t > Traits::MIN_TIME
#ifndef Q_WS_WIN
        && t < Traits::MAX_TIME // FIXME sth must be broken on windows.
#endif // Q_WS_WIN
        ) {
      if (max < t)
        max = t;
      if (min > t)
        min = t;
    }
  }
  qint64 range = max - min;
  if (range < 0)
    range = 0;

  int metric = (range / 86400 + 1) * 300; // at least 5min / day
  Q_ASSERT(metric);
  if (metric > 86400)
    metric = 86400;

  typedef QHash<qint64, int> Histogram;
  Histogram hist;
  qint64 maxX = max / metric,
         minX = min / metric;
  foreach (const Annotation &a, l) {
    qint64 x = a.createTime() / metric;
    if (x <= 0)
      continue;
     ++hist[x];
  }
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

  DOUT("hash size =" << hist.size());
  DOUT("maxY =" << maxY);

  int rangeX = maxX - minX;

  int dailyStride = 86400 / metric;
  if (rangeX < dailyStride) {
    rangeX = dailyStride;
    maxX = minX + dailyStride;
  }
  Q_ASSERT(rangeX);

  // Draw histogram
  int width = view.width() ? view.width() : p.device()->width(),
      height = view.height() ? view.height() : p.device()->height();
  int histHeight = height - (MarginSize + LabelHeight);
  for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
    qint64 x = i.key();
    int y = i.value();

    qreal px = (x - minX) / qreal(rangeX),
          py = y / qreal(maxY);
    x = width * px;
    y = histHeight * py;

    int r = r_start + (r_stop - r_start) * py,
        g = g_start + (g_stop - g_start) * py,
        b = b_start + (b_stop - b_start) * py;
    QColor color(r, g, b, alpha);

    p.setPen(color);
    p.drawLine(view.x() + x, view.y() + histHeight,
               view.x() + x, view.y() + histHeight - y);
  }

  // Draw axis
  paintAxis(p, QRect(view.x(), view.y(), width, histHeight), maxY);

  // Calculate local peaks
  QList<QPoint> peaks;
  {
    enum { distanceX = 30 }; // 5min * 30 = 2.5h
    int t = maxY;
    for (int i = TopCount -1; i>=0; i--)
      if (top[i]) {
        t = top[i];
        break;
      }
    int limitY = t / 2 + 1;
    Q_ASSERT(limitY);
    for (Histogram::ConstIterator i = hist.begin(); i != hist.end(); ++i) {
      qint64 x = i.key();
      int y = i.value();
      if (y < limitY)
        continue;
      if (!peaks.isEmpty() &&
          peaks.last().x() + distanceX > x) {
        if (peaks.last().y() < y)
          peaks.last() = QPoint(x, y);
      } else
        peaks.append(QPoint(x, y));
    }
  }

  DOUT("peaks =" << peaks);

  // Draw peaks
  if (!peaks.isEmpty()) {
    enum { LabelRotation = -20 };
    QFont f = p.font();
    f.setPointSize(PeakFontSize);
#ifdef Q_WS_WIN
    f.setBold(false);
#else
    f.setBold(true);
#endif // Q_WS_WIN
    p.setFont(f);
    p.setPen(Qt::green);

    foreach (const QPoint &peak, peaks) {
      qreal px = (peak.x() - minX) / qreal(rangeX),
            py = peak.y() / qreal(maxY);
      int x = width * px,
          y = histHeight * py;

      x = view.x() + x,
      y = view.y() + histHeight - y;

      qint64 secs = peak.x() * metric;
      QDateTime ts = QDateTime::fromMSecsSinceEpoch(secs * 1000);
      QString pos = ts.toString("h:mm ddd");
      QString label = QString("<%1%,%2")
          .arg(QString::number(peak.y() * 100 / maxY))
          .arg(pos);
      if (peak.y() >= maxY * 0.9)
        p.drawText(QRect(x, y, x + LabelWidth, y + PeakFontHeight), label);
      else {
        p.save();
        p.translate(x, y);
        p.rotate(LabelRotation);
        p.drawText(0, 0, label);
        p.restore();
      }
    }
  }

  // Draw label
  {
    p.setPen(Qt::cyan);
    int y = view.y() + height - LabelHeight - 3;
    bool drawHour = rangeX <= dailyStride * 7; // one week
    bool drawWeek = !drawHour && rangeX < dailyStride * 180; // half a year
    int labelRotation = drawHour ? 60 : 30;
    QString fmt = drawHour ? "M/d ddd" : "M/d/yyyy";

    enum { HourMetric = 4 }; // draw every 4 hour

    QFont f = p.font();
    f.setPointSize(DateFontSize);
#ifdef Q_WS_WIN
    f.setBold(false);
#else
    f.setBold(true);
#endif // Q_WS_WIN
    p.setFont(f);

    int hourCount = range / 3600 + 1;
    Q_ASSERT(hourCount);
    for (int i = 0; i < hourCount; i++) {
      qint64 secs = minX * metric + i * 3600;
      QDateTime ts = QDateTime::fromMSecsSinceEpoch(secs * 1000);
      int h = ts.time().hour();
      int d = ts.date().day();
      int x = view.x() + width * i / hourCount;
      if (!h &&
          (drawHour || drawWeek && d%7 == 1 || d == 1)) {
        QString date = ts.toString(fmt);
        p.save();
        p.translate(x, y);
        p.rotate(labelRotation);
        p.drawText(0, 0, date);
        p.restore();
      } else if (drawHour && h % HourMetric == HourMetric / 2) {
        p.save();
        QFont f = p.font();
        f.setPointSize(HourFontSize);
        p.setFont(f);
        QString hour = QString("%1:00").arg(QString::number(h));
        p.translate(x, y);
        p.rotate(labelRotation);
        p.drawText(0, 0, hour);
        p.restore();
      }
    }
  }

  // Draw title
  if (!title.isEmpty()) {
    enum { TitleFontSize = 20, NoteFontSize = 10 };
    enum { TitleHeight = TitleFontSize, NoteHeight = NoteFontSize, NoteMargin = 5 };

    int titleHeight = qMin(height, int(TitleHeight));
    {
      QFont f = p.font();
      f.setBold(true);
      f.setPointSize(TitleFontSize);
      p.setFont(f);
      p.setPen(Qt::cyan);
      p.drawText(QRect(0, 0, width, titleHeight), Qt::AlignCenter, title);
    }

    if (height > TitleHeight + NoteHeight + NoteMargin) {
      QString peak = QString::number(maxY),
              averagePerDay = QString::number(l.size()*(86400/metric)/qreal(rangeX), 'f', 2),
              unit = metric < 3600 + 1800 ? QString("%1 min.").arg(QString::number(metric / 60)) :
                     metric < 86400 ? QString("%1 hrs.").arg(QString::number(metric/3600.0, 'f', 1)) :
                     QString("day");
      QString note = QString("peak = %1 / %2   average = %3 / day")
                     .arg(peak).arg(unit).arg(averagePerDay);
      QFont f = p.font();
      f.setBold(true);
      f.setItalic(ITALIC);
      f.setPointSize(NoteFontSize);
      p.setFont(f);
      p.setPen(QColor("orange"));
      p.drawText(QRect(0, TitleHeight + NoteMargin, width, NoteHeight), Qt::AlignCenter, note);
    }
  }
}

void
AnnotCloud::
AnnotationPainter::paintHistogramByUserId(QPainter &p, const AnnotationList &l, const QString &title, const QRect &view) const
{
  DOUT("count =" << l.size());

  if (l.isEmpty())
    return;
  enum { LabelFontSize = 10, PeakFontSize = 8 }; // 10em
  enum { LabelHeight = LabelFontSize + 2, PeakFontHeight = PeakFontSize + 2 };
  enum { MarginSize = 3 };
  enum { LabelWidth = 50 };

  int maxY = 1;

  typedef QHash<qint64, int> H;
  H h;
  foreach (const Annotation &a, l) // skip anonymous user, skip UI_Guest here in the future
    if (a.createTime() > Traits::MIN_TIME
#ifndef Q_WS_WIN
        && a.createTime() < Traits::MAX_TIME // FIXME sth must be broken on windows.
#endif // Q_WS_WIN
        ) {
      int y = ++h[a.userId()];
      if (y > maxY)
        maxY = y;
    }

  DOUT("hash size =" << h.size());
  DOUT("maxY =" << maxY);

  // Sort hash by value
  typedef QMultiMap<int, qint64> M;
  M m;
  for (H::ConstIterator i = h.begin(); i != h.end(); ++i)
    m.insert(i.value(), i.key());

  int rangeX = m.size();

  // Draw histogram and label
  int width = view.width() ? view.width() : p.device()->width(),
      height = view.height() ? view.height() : p.device()->height();
  int histHeight = height - (MarginSize + LabelHeight);

  QFont labelFont = p.font();
  labelFont.setPointSize(LabelFontSize);
#ifdef Q_WS_WIN
  labelFont.setBold(false);
#else
  labelFont.setBold(true);
#endif // Q_WS_WIN
  p.setFont(labelFont);
  int labelCount = qMax(1, width/LabelWidth);
  int stride = rangeX / labelCount;
  if (!stride)
    stride = rangeX;

  int labelY = view.y() + height - LabelHeight;
  int i = 0;
  foreach (int key, QtExt::revertList(m.keys())) {
    { // Histogram
      int x = i,
          y = key;
      qreal px = x / qreal(rangeX),
            py = y / qreal(maxY);
      x = width * px;
      y = histHeight * py;

      int r = r_start + (r_stop - r_start) * py,
          g = g_start + (g_stop - g_start) * py,
          b = b_start + (b_stop - b_start) * py;
      QColor color(r, g, b, alpha);

      p.setPen(color);
      p.drawLine(view.x() + x, view.y() + histHeight,
                 view.x() + x, view.y() + histHeight - y);
    }

    // Label, skip the first point
    if (i % stride == 0) {
      int labelCount = i / stride;
      QString label = QString("%2(%1)")
          .arg(QString::number(i+1))  // x:user count
          .arg(QString::number(key)); // y:annot count
      p.setPen(Qt::cyan);
      int labelX = view.x() + MarginSize + labelCount * LabelWidth;
      if (labelX >= view.right())
        p.drawText(QRect(labelX, labelY, LabelWidth, LabelHeight), label);
    }
    i++;
  }

  // Draw axis
  paintAxis(p, QRect(view.x(), view.y(), width, histHeight), maxY);

  // Draw peak
  {
    QFont f = p.font();
    f.setPointSize(PeakFontSize);
    p.setFont(f);
    p.setPen(Qt::green);
    int x = view.x() + MarginSize,
        y = view.y();
    QString label = QString("<100%,%1").arg(QString::number(maxY));
    p.drawText(QRect(x, y, x + LabelWidth, y + PeakFontHeight), label);
  }

  // Draw title
  if (!title.isEmpty()) {
    enum { TitleFontSize = 20, NoteFontSize = 10 };
    enum { TitleHeight = TitleFontSize, NoteHeight = NoteFontSize, NoteMargin = 5 };

    int titleHeight = qMin(height, int(TitleHeight));
    {
      QFont f = p.font();
      f.setBold(true);
      f.setPointSize(TitleFontSize);
      p.setFont(f);
      p.setPen(Qt::cyan);
      p.drawText(QRect(0, 0, width, titleHeight), Qt::AlignCenter, title);
    }

    if (height > TitleHeight + NoteHeight + NoteMargin) {
      QString peak = QString::number(maxY),
              average = QString::number(l.size()/qreal(rangeX), 'f', 2),
              total = QString::number(h.size());
      QString note = QString("peak = %1 / user   average = %2 / user   total = %3 users")
                     .arg(peak).arg(average).arg(total);
      QFont f = p.font();
      f.setBold(true);
      f.setItalic(ITALIC);
      f.setPointSize(NoteFontSize);
      p.setFont(f);
      p.setPen(QColor("orange"));
      p.drawText(QRect(0, TitleHeight + NoteMargin, width, NoteHeight), Qt::AlignCenter, note);
    }
  }
}

// See: http://harmattan-dev.nokia.com/docs/library/html/qt4/painting-svggenerator.html
bool
AnnotCloud::
AnnotationPainter::saveHistogramAsFile(
    const QString &fileName,
    const AnnotationList &l, Annotation::Field sortBy,
   int width, int height,
    const QString &title, const QString &description) const
{
  enum { CanvasWidth = 400, CanvasHeight = 200 };
#ifdef WITH_QT_SVG
  DOUT("enter: fileName =" << fileName);

  if (width <= 0)
    width = CanvasWidth;
  if (height <= 0)
    height = CanvasHeight;

  QSvgGenerator generator;
  generator.setFileName(fileName);
  generator.setSize(QSize(width, height));
  generator.setViewBox(QRect(0, 0, width, height));
  if (!title.isEmpty())
    generator.setTitle(title);
  if (!description.isEmpty())
    generator.setDescription(description);

  QPainter painter(&generator);
  painter.setRenderHints(
    //QPainter::Antialiasing |
    QPainter::TextAntialiasing |
    QPainter::SmoothPixmapTransform
  );

  painter.fillRect(QRect(0, 0, width, height), Qt::transparent);
  paintHistogram(painter, l, sortBy, title);

  DOUT("exit");
  return true;
#else
  Q_UNUSED(fileName)
  Q_UNUSED(width) Q_UNUSED(height)
  Q_UNUSED(l) Q_UNUSED(sortBy)
  Q_UNUSED(title) Q_UNUSED(description)
  return false;
#endif // WITH_QT_SVG
}

// EOF
