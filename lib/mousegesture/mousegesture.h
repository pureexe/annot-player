#ifndef MOUSEGESTURE_H
#define MOUSEGESTURE_H

// mousegesture.h
// 4/25/2012

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

/**
 *  A mouse gesture is a list of Directions that
 *  trigger the gestured signal. Create instances
 *  and add to a MouseGestureFilter object.
 */
class MouseGesture : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MouseGesture)
  typedef MouseGesture Self;
  typedef QObject Base;
public:
  /**
  *  A Direction can be any of the following:
  *
  *  Up
  *  Down
  *  Left
  *  Right
  *  AnyHorizontal (Left or Right)
  *  AnyVertical (Up or Down)
  *
  *  In addition to these, the NoMatch enum is
  *  available. A gesture holding only the NoMatch
  *  enum is gestured when no other gesture can be
  *  matched to the mouse gesture performed.
  */
  enum Direction { NoMatch, Up, Down, Left, Right, Horizontal, Vertical,
                   DirectionCount };
  typedef QList<Direction> DirectionList;

public:
  explicit MouseGesture(const DirectionList &directions, const QString &description = QString(), QObject *parent = nullptr)
    : Base(parent), directions_(directions), description_(description) { }

  const DirectionList &directions() const { return directions_; }
  const QString description() const { return description_; }

  static QString directionToString(Direction direction);
  static QString directionsToString(const DirectionList &l, const QString &sep = " ");

signals:
  void triggered();
  void message(QString msg);
public slots:
  void trigger();
  void setDescription(const QString &t) { description_ = t; }

private:
  DirectionList directions_;
  QString description_;
};

typedef QList<MouseGesture *> MouseGestureList;

#endif // MOUSEGESTURE_H
