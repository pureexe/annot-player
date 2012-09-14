#ifndef ACHORIZONTALSLIDER_H
#define ACHORIZONTALSLIDER_H

// achorizontalslider.h
// 5/16/2012

#include "src/common/acslider.h"
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class AcHorizontalSlider : public AcSlider
{
  Q_OBJECT
  Q_PROPERTY(qreal grooveHeight READ grooveHeight WRITE setGrooveHeight)
  Q_DISABLE_COPY(AcHorizontalSlider)
  typedef AcHorizontalSlider Self;
  typedef AcSlider Base;

  qreal grooveHeight_,
        minimumGrooveHeight_,
        maximumGrooveHeight_;
  QPropertyAnimation *grooveHeightAni_;

public:
  explicit AcHorizontalSlider(QWidget *parent = nullptr);

  // - Properties -

  qreal grooveHeight() const { return grooveHeight_; }
  qreal minimumGrooveHeight() const { return minimumGrooveHeight_; }
  qreal maximumGrooveHeight() const { return maximumGrooveHeight_; }
public slots:
  void setGrooveHeight(qreal value);
  void setMinimumGrooveHeight(qreal value) { minimumGrooveHeight_ = value; }
  void setMaximumGrooveHeight(qreal value) { maximumGrooveHeight_ = value; }

protected slots:
  void updateStyleSheet();

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void changeEvent(QEvent *event) override;
};

#endif // ACHORIZONTALSLIDER_H
