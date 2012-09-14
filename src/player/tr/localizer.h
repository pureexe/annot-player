#ifndef LOCALIZER_H
#define LOCALIZER_H

// localizer.h
// 8/7/2011

#include <QObject>

class Localizer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Localizer)
  typedef Localizer Self;
  typedef QObject Base;
protected:
  explicit Localizer(QObject *parent = nullptr) : Base(parent) { }
public:
  static QString translate(int id);
};

#endif // LOCALIZER_H
