#ifndef ACPROTOCOL_H
#define ACPROTOCOL_H

// acprotocol.h
// 6/20/2012

//#define AC_MESSAGE_SIGNALS \
//  void message(const QString &text); \
//  void errorMessage(const QString &text); \
//  void warning(const QString &text); \
//  void notification(const QString &text);
//
//#define AC_MESSAGE_SLOTS \
//  void showMessage(const QString &text); \
//  void showError(const QString &text); \
//  void warn(const QString &text); \
//  void notify(const QString &text);

#define AC_CONNECT_MESSAGE(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(message(QString)), _target, SLOT(showMessage(QString)), _type);
#define AC_CONNECT_ERROR(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(errorMessage(QString)), _target, SLOT(showError(QString)), _type);
#define AC_CONNECT_WARNING(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(warning(QString)), _target, SLOT(warn(QString)), _type);
#define AC_CONNECT_NOTIFICATION(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(notification(QString)), _target, SLOT(notify(QString)), _type);

#define AC_CONNECT_MESSAGES(_source, _target, _type) \
  AC_CONNECT_MESSAGE(_source, _target, _type) \
  AC_CONNECT_ERROR(_source, _target, _type) \
  AC_CONNECT_WARNING(_source, _target, _type) \
  AC_CONNECT_NOTIFICATION(_source, _target, _type)

#define AC_FORWARD_MESSAGE(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(message(QString)), _target, SIGNAL(message(QString)), _type);
#define AC_FORWARD_ERROR(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(errorMessage(QString)), _target, SIGNAL(errorMessage(QString)), _type);
#define AC_FORWARD_WARNING(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(warning(QString)), _target, SIGNAL(warning(QString)), _type);
#define AC_FORWARD_NOTIFICATION(_source, _target, _type) \
  QObject::connect(_source, SIGNAL(notification(QString)), _target, SIGNAL(notification(QString)), _type);

#define AC_FORWARD_MESSAGES(_source, _target, _type) \
  AC_FORWARD_MESSAGE(_source, _target, _type) \
  AC_FORWARD_ERROR(_source, _target, _type) \
  AC_FORWARD_WARNING(_source, _target, _type) \
  AC_FORWARD_NOTIFICATION(_source, _target, _type)

#endif // ACPROTOCOL_H
