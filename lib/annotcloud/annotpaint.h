#ifndef _ANNOTCLOUD_ANNOTPAINT_H
#define _ANNOTCLOUD_ANNOTPAINT_H

// annotcloud/annotpaint.h
// 4/6/2012

#include "lib/annotcloud/annotation.h"
#include <QtCore/QObject>
#include <QtCore/QRect>

QT_FORWARD_DECLARE_CLASS(QPainter)

namespace AnnotCloud {

class AnnotationPainter : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationPainter)
  typedef AnnotationPainter Self;
  typedef QObject Base;

  // - Constructions -
public:
  static Self *globalInstance() { static Self g; return &g; }

  explicit AnnotationPainter(QObject *parent = nullptr)
    : Base(parent) { }

public:
  void paintHistogram(QPainter &painter, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                      const QString &title = QString(), const QRect &view = QRect()) const;
  bool saveHistogramAsFile(const QString &fileName, const AnnotationList &l, Annotation::Field sortBy = Annotation::Pos,
                           int width = 0, int height = 0,
                           const QString &title = QString(), const QString &description = QString()) const;
protected:
  void paintHistogramByPos(QPainter &painter, const AnnotationList &l, const QString &title = QString(), const QRect &view = QRect()) const;
  void paintHistogramByCreateTime(QPainter &painter, const AnnotationList &l, const QString &title = QString(), const QRect &view = QRect()) const;
  void paintHistogramByUserId(QPainter &painter, const AnnotationList &l, const QString &title = QString(), const QRect &view = QRect()) const;

private:
  void paintAxis(QPainter &painter, const QRect &view, int maxY) const;
};

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_ANNOTPAINT_H
