#ifndef ANNOTATIONGRAPHICSITEM_H
#define ANNOTATIONGRAPHICSITEM_H

// annotationgraphicsitem.h
// 7/16/2011

#include "module/annotcloud/annotation.h"
#include <QtGui/QGraphicsTextItem>

QT_BEGIN_NAMESPACE
class QAction;
class QContextMenuEvent;
//class QGraphicsView;
class QGraphicsScene;
class QMouseEvent;
class QPoint;
class QPropertyAnimation;
class QStringList;
class QTimer;
QT_END_NAMESPACE

class SignalHub;
class AnnotationGraphicsView;

class AnnotationGraphicsItem : public QGraphicsTextItem
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationGraphicsItem)
  Q_PROPERTY(QPointF pos READ pos WRITE setPos)
  Q_PROPERTY(QPointF relativePos READ relativePos WRITE setRelativePos)
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

  typedef AnnotationGraphicsItem Self;
  typedef QGraphicsTextItem Base;

  typedef AnnotCloud::Annotation Annotation;

public:
  enum { AnnotationGraphicsItemType = UserType + 1 };
  enum Style { FloatStyle = 0, FlyStyle, SubtitleStyle, TopStyle, BottomStyle };  // Appear style

  static void warmUp(); ///< optional, caching fonts on first load

  enum Effect { DefaultEffect = 0, TransparentEffect, ShadowEffect, BlurEffect, EffectCount };

public:
  explicit AnnotationGraphicsItem(SignalHub *hub, AnnotationGraphicsView *viewWithScene);

  const Annotation &annotation() const { return annot_; }
  Annotation &annotation() { return annot_; }

  void setAnnotation(const Annotation &annot);
  void invalidateAnnotation();

  const QString &richText() const { return richText_; } ///< tidied HTML

  bool autoDelete() const { return autoDelete_; }

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

  QPointF relativePos() const { return pos() - origin_; }

  qreal opacity() const { return opacity_; }

protected:
  //QString parse(const QString &text);
  static bool isSubtitle(const QString &text);

public slots:
  void setVisible(bool t) { Base::setVisible(t); }
  void setRelativePos(const QPointF &offset) { setPos(origin_ + offset); }
  void setScale(qreal value) { Base::setScale(value); }
  void showMe();   // Add me to graphics scene, and autmatic remove me.
  void selectMe();
  void updateEffect();
  void setEffect(Effect e);

  void reset();
  void pause();
  void resume();
  void escapeFrom(const QPointF &pos);
  void rushTo(const QPointF &pos);

  void setOpacity(qreal value);

  void disappear();
protected slots:
  void addMe();
  void removeMe(); // Remove me from graphics scene
  void deleteMe(); // Delete corresponding annotation

  void updateOpacity();
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
  void fly(const QPointF &from, const QPointF &to, int msecs);
  void escapeTo(const QPointF &pos, int msecs);
  void rushTo(const QPointF &pos, int msecs);
  void stay(const QPointF &pos, int msecs);
  void appear(const QPointF &pos, int msecs);

  int flyTime() const;  ///< in msecs
  int stayTime(Style style) const; ///< in msecs

  void fadeIn(int msecs);
  void fadeOut(int msecs);

  // Events:
public:
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override
  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \override
  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
protected:
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event); ///< \override
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event); ///< \override
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event); ///< \override

  //virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event); ///< \override
  //virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event); ///< \override
  //virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event); ///< \override

private:
  void setDefaultStyle();

  int nextY(int visibleTime, Style style) const;

private:
  bool autoDelete_;
  Annotation annot_;

  QPointF origin_;
  AnnotationGraphicsView *view_;
  QGraphicsScene *scene_;
  SignalHub *hub_;

  Style style_;
  QTimer *removeLaterTimer_;
  QPropertyAnimation *flyAni_,*flyOpacityAni_,
                     *escapeAni_, *rushAni_,
                     *appearOpacityAni_, *fadeAni_;

  QPoint dragPos_;
  bool dragPaused_;
  QString richText_;
  qreal opacity_;
};

#endif // ANNOTATIONGRAPHICSITEM_H
