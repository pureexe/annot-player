#ifndef POSITIONSLIDER_H
#define POSITIONSLIDER_H

// positionslider.h
// 3/20/2012

#include "project/common/acslider.h"

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class PositionSlider : public AcSlider
{
  Q_OBJECT
  Q_PROPERTY(qreal grooveHeight READ grooveHeight WRITE setGrooveHeight)
  Q_PROPERTY(qreal handleRadius READ handleRadius WRITE setHandleRadius)
  Q_DISABLE_COPY(PositionSlider)
  typedef PositionSlider Self;
  typedef AcSlider Base;

  int availablePosition_;
  qreal grooveHeight_;
  qreal handleRadius_;
  QPropertyAnimation *grooveHeightAni_;
  QPropertyAnimation *handleRadiusAni_;

public:
  explicit PositionSlider(QWidget *parent = 0);

  // - Properties -

  int availablePosition() const { return availablePosition_; }
  qreal grooveHeight() const { return grooveHeight_; }
  qreal handleRadius() const { return handleRadius_; }
public slots:
  void setGrooveHeight(qreal value);
  void setHandleRadius(qreal value);
  void clearAvailablePosition() { setAvailablePosition(0); }

  void setAvailablePosition(int value);
protected slots:
  void updateStyleSheet(int value = 0);

  // - Events -
protected:
  virtual void changeEvent(QEvent *event); ///< \override
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
};

#endif // POSITIONSLIDER_H
