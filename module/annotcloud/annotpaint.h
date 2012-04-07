#ifndef _ANNOTCLOUD_ANNOTPAINT_H
#define _ANNOTCLOUD_ANNOTPAINT_H

// annotcloud/annotpaint.h
// 4/6/2012

#include "module/annotcloud/annotation.h"
#include <QRect>
#include <QColor>
#include <QObject>
QT_FORWARD_DECLARE_CLASS(QPainter)

namespace AnnotCloud {

class AnnotationPainter : public QObject
{
  Q_OBJECT
  typedef AnnotationPainter Self;
  typedef QObject Base;

  // - Constructions -
public:
  static Self *globalInstance() { static Self g; return &g; }

  explicit AnnotationPainter(QObject *parent = 0)
    : Base(parent) { }

public:
  void paintTitle(QPainter &painter, const QString &text,
                  const QColor &color = QColor(), int fontSize = 0,
                  const QRect &canvas = QRect()) const;
  void paintHistogram(QPainter &painter, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                      const QString &title = QString(), const QRect &canvas = QRect()) const;
  bool saveHistogramAsFile(const QString &fileName, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                           int width = 0, int height = 0,
                           const QString &title = QString(), const QString &description = QString()) const;
protected:
  void paintHistogramByPos(QPainter &painter, const AnnotationList &l, const QString &title = QString(), const QRect &canvas = QRect()) const;
  void paintHistogramByCreateTime(QPainter &painter, const AnnotationList &l, const QString &title = QString(), const QRect &canvas = QRect()) const;
};

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_ANNOTPAINT_H
