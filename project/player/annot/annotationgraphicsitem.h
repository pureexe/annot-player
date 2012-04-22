#ifndef ANNOTATIONGRAPHICSITEM_H
#define ANNOTATIONGRAPHICSITEM_H

// annotationgraphicsitem.h
// 7/16/2011

#include "module/annotcloud/annotation.h"
#include <QtCore/QEasingCurve>
#include <QtGui/QGraphicsTextItem>

QT_BEGIN_NAMESPACE
//class QGraphicsView;
class QGraphicsScene;
class QPoint;
class QTimer;
class QPropertyAnimation;
class QAction;
class QStringList;
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

  typedef AnnotCloud::Annotation Annotation;

  Annotation annot_;

public:
  enum { AnnotationGraphicsItemType = UserType + 1 };
  enum Style { FloatStyle = 0, FlyStyle, SubtitleStyle, TopStyle, BottomStyle };  // Appear style

  static void warmUp(); ///< optional, caching fonts on first load

  enum Effect { DefaultEffect = 0, TransparentEffect, ShadowEffect, BlurEffect, EffectCount };

public:
  AnnotationGraphicsItem(const Annotation &annot, SignalHub *hub, AnnotationGraphicsView *viewWithScene);
  //explicit AnnotationGraphicsItem(AnnotationGraphicsView *viewWithScene);

  const Annotation &annotation() const { return annot_; }
  Annotation &annotation() { return annot_; }

  void setAnnotation(const Annotation &annot);
  void invalidateAnnotation();

  const QString &richText() const { return richText_; } ///< tidied HTML

  ///  Override
  virtual int type() const { return AnnotationGraphicsItemType; }
  Style style() const { return style_; }
  void setStyle(Style style) { style_ = style; }

  bool isPaused() const;

  //  \override  Make the item easier to be selected. - TODO: It is better to do this at viewer level rather the messing up the item itself.
  //virtual bool contains(const QPointF &point) const
  //{ return Base::boundingRect().contains(point); }

  bool isSubtitle() const { return style_ == SubtitleStyle; }

  QString abstract() const;

protected:
  //QString parse(const QString &text);
  static bool isSubtitle(const QString &text);

public slots:
  void setScale(qreal value) { Base::setScale(value); }
  void addMe();
  void removeMe(); // Remove me from graphics scene
  void deleteMe(); // Delete corresponding annotation
  void showMe();   // Add me to graphics scene, and autmatic remove me.
  void invalidateEffect();
  void setEffect(Effect e);

  void pause();
  void resume();

protected slots:
  void fly();
  void stay(Style location = TopStyle);

  void removeLater(int msecs);

  void edit();
  void copyToClipboard() const;

  void setText(const QString &text); ///< The default way to set text
  void setTags(const QStringList &tags); ///< Apply tags.

  void blessMe();
  void curseMe();
  void blockMe();
  void blockUser();

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

  int nextY(int visibleTime, Style style) const;

private:
  AnnotationGraphicsView *view_;
  QGraphicsScene *scene_;
  SignalHub *hub_;

  QTimer *removeLaterTimer_;
  Style style_;
  QPropertyAnimation *ani_;

  QPoint dragPos_;
  QString richText_;
};

#endif // ANNOTATIONGRAPHICSITEM_H
