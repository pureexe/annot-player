#ifndef FADEANIMATION_H
#define FADEANIMATION_H

#include <QtCore/QEasingCurve>
#include <QtCore/QPropertyAnimation>

class FadeAnimation : public QPropertyAnimation
{
  Q_OBJECT
  Q_DISABLE_COPY(FadeAnimation)
  typedef FadeAnimation Self;
  typedef QPropertyAnimation Base;

public:
  explicit FadeAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = nullptr)
    : Base(target, propertyName, parent) { }

public:
  void fadeIn(qreal opacity = 1.0);
  void fadeOut(qreal opacity = 1.0);

protected:
  void restart(qreal start, qreal end, qint64 duration, QEasingCurve::Type curve);
};

#endif // FADEANIMATION_H
