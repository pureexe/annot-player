#ifndef _CORE_USER_H
#define _CORE_USER_H

// core/user.h
// 7/4/2011
// Describe user information.

#include <QObject>

namespace Core {

  class User : public QObject
  {
    Q_OBJECT
    typedef User Self;
    typedef QObject Base;

  public:
    explicit User(QObject *parent = nullptr) : Base(parent) { }

    // Properties:
  public:

  };

} // namespace Core

#endif // _CORE_USER_H
