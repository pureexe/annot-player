#ifndef EMBEDDEDCANVAS_H
#define EMBEDDEDCANVAS_H

// embeddedcanvas.h
// 4/8/2012

#include "lib/annotcloud/annotation.h"
#include <QtGui/QWidget>

QT_FORWARD_DECLARE_CLASS(QPainter)

class DataManager;
class SignalHub;
class Player;

class EmbeddedCanvas : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(EmbeddedCanvas)
  typedef EmbeddedCanvas Self;
  typedef QWidget Base;

public:
  EmbeddedCanvas(DataManager *data, SignalHub *hub, Player *player, QWidget *parent = nullptr);

signals:
  void enabledChanged(bool enabled);
  void visibleChanged(bool visible);

public:
  bool isEnabled() const { return enabled_; }
  //bool isEmpty() const;
  bool needsDisplay() const;
public slots:
  void invalidatePaint() { if (isVisible()) repaint(); }

  void setEnabled(bool t);
  void updateVisible();

  void setUserIds(const QList<qint64> userIds) { userIds_ = userIds; invalidatePaint(); }
  void clearUserIds() { userIds_.clear(); invalidatePaint(); }

  // - Events -
public slots:
  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  void setVisible(bool visible) override;
protected:
  void paintEvent(QPaintEvent *event) override;

  // - Paint -
protected:
  void paintHistogram(QPainter &painter, const QRect &view, const AnnotCloud::AnnotationList &l);
  //void paintCoordinate(QPainter &painter, const QRect &view);

  static void drawCross(QPainter &painter, const QPoint &center, int size = 5);

private:
  bool enabled_;
  DataManager *data_;
  SignalHub *hub_;
  Player *player_;

  QList<qint64> userIds_;
};

#endif // EMBEDDEDCANVAS_H
