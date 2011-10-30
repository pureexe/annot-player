#ifndef ANNOTATIONGRAPHICSITEM_H
#define ANNOTATIONGRAPHICSITEM_H

// annotationgraphicsitem.h
// 7/16/2011

#include "core/cloud/annotation.h"
#include <QGraphicsTextItem>
#include <QEasingCurve>
#include <QStringList>

QT_BEGIN_NAMESPACE
//class QGraphicsView;
class QGraphicsScene;
class QPoint;
class QTimer;
class QPropertyAnimation;
class QAction;
class QContextMenuEvent;
class QMouseEvent;
QT_END_NAMESPACE

class SignalHub;
class AnnotationGraphicsView;

class AnnotationGraphicsItem : public QGraphicsTextItem
{
  Q_OBJECT
  Q_PROPERTY(QPointF pos READ pos WRITE setPos)

  typedef AnnotationGraphicsItem Self;
  typedef QGraphicsTextItem Base;

  typedef Core::Cloud::Annotation Annotation;

  Annotation annotation_;

public:
  enum { AnnotationGraphicsItemType = UserType + 1 };
  enum Style { FlyStyle = 0, SubtitleStyle, TopStyle, BottomStyle };  // Appear style

public:
  explicit AnnotationGraphicsItem(const Annotation &annotation, SignalHub *hub, AnnotationGraphicsView *viewWithScene);
  //explicit AnnotationGraphicsItem(AnnotationGraphicsView *viewWithScene);

  const Annotation &annotation() const;
  void setAnnotation(const Annotation &annot);

  ///  Override
  virtual int type() const { return AnnotationGraphicsItemType; }
  Style style() const;
  void setStyle(Style style);

  bool isPaused() const;

  //  \override  Make the item easier to be selected. - TODO: It is better to do this at viewer level rather the messing up the item itself.
  //virtual bool contains(const QPointF &point) const
  //{ return Base::boundingRect().contains(point); }

protected:
  //QString parse(const QString &text);

public slots:
  void addMe();
  void removeMe(); // Remove me from graphics scene
  void showMe();   // Add me to graphics scene, and autmatic remove me.

protected slots:
  void pause();
  void resume();

  void fly();
  void stay(Style location = TopStyle);

  void autoRemove(int msecs = 0);

  void edit();
  void copyToClipboard() const;

  void setText(const QString &text); ///< The default way to set text
  void setTags(const QStringList &tags); ///< Apply tags.

protected:
  void fly(const QPointF &from, const QPointF &to, int msecs,
           QEasingCurve::Type type = QEasingCurve::Linear);
  void stay(const QPointF &pos, int msecs);

  int flyTime() const;  ///< in msecs
  int stayTime(Style style) const; ///< in msecs

  // Events:
public:
  void contextMenuEvent(QContextMenuEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
  void setDefaultStyle();

private:
  AnnotationGraphicsView *view_;
  QGraphicsScene *scene_;
  SignalHub *hub_;

  QTimer *autoRemoveTimer_;
  Style style_;
  QPropertyAnimation *ani_;

  QPoint dragPos_;
};

#endif // ANNOTATIONGRAPHICSITEM_H
