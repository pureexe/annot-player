#ifndef ACHORIZONTALSLIDER_H
#define ACHORIZONTALSLIDER_H

// achorizontalslider.h
// 5/16/2012

#include "module/qtext/slider.h"
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class AcHorizontalSlider : public QtExt::Slider
{
  Q_OBJECT
  Q_PROPERTY(qreal grooveHeight READ grooveHeight WRITE setGrooveHeight)
  Q_DISABLE_COPY(AcHorizontalSlider)
  typedef AcHorizontalSlider Self;
  typedef QtExt::Slider Base;

  qreal grooveHeight_,
        minimumGrooveHeight_,
        maximumGrooveHeight_;
  QPropertyAnimation *grooveHeightAni_;

public:
  explicit AcHorizontalSlider(QWidget *parent = 0);

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
  virtual void enterEvent(QEvent *event); ///< \override
  virtual void leaveEvent(QEvent *event); ///< \override
  virtual void changeEvent(QEvent *event); ///< \override
};

#endif // ACHORIZONTALSLIDER_H
