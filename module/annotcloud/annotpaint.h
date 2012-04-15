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
  void paintHistogram(QPainter &painter, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                      const QString &title = QString::null, const QRect &view = QRect()) const;
  bool saveHistogramAsFile(const QString &fileName, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                           int width = 0, int height = 0,
                           const QString &title = QString::null, const QString &description = QString::null) const;
protected:
  void paintHistogramByPos(QPainter &painter, const AnnotationList &l, const QString &title = QString::null, const QRect &view = QRect()) const;
  void paintHistogramByCreateTime(QPainter &painter, const AnnotationList &l, const QString &title = QString::null, const QRect &view = QRect()) const;
  void paintHistogramByUserId(QPainter &painter, const AnnotationList &l, const QString &title = QString::null, const QRect &view = QRect()) const;

private:
  void paintAxis(QPainter &painter, const QRect &view, int maxY) const;
};

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_ANNOTPAINT_H
