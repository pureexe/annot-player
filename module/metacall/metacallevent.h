#ifndef METACALLEVENT_H
#define METACALLEVENT_H

// metacallevent.h
// 4/12/2012
// Must be consistent with corelib/kernel/qobject_p.h
// Last checked with Qt 4.7.4 and 4.8.1.

#include <QtCore/QEvent>
#include <QtCore/QMetaObject>

QT_FORWARD_DECLARE_CLASS(QSemaphore)
QT_FORWARD_DECLARE_CLASS(QObject)

#if QT_VERSION >= 0x040800
#  define MCE_HAS_STATIC_CALL
#endif // QT_VERSION

QT_BEGIN_NAMESPACE

#ifdef MCE_HAS_STATIC_CALL

class QMetaCallEvent : public QEvent
{
public:
  ///< Must be consisitent with that in QObjectPrivate
  typedef void (*StaticMetaCallFunction)(QObject *, QMetaObject::Call, int, void **);

  QMetaCallEvent(ushort method_offset, ushort method_relative, StaticMetaCallFunction callFunction,
                 const QObject *sender, int signalId,
                 int nargs = 0, int *types = 0, void **args = 0, QSemaphore *semaphore = 0);
  ~QMetaCallEvent();
  virtual void placeMetaCall(QObject *object);

public:
  const QObject *sender() const { return sender_; }
  int signalId() const { return signalId_; }
  int nargs() const { return nargs_; }
  int *types() const { return types_; }
  void **args() const { return args_; }

  int id() const { return method_offset_ + method_relative_; } // hash

private:
  const QObject *sender_;
  int signalId_;
  int nargs_;
  int *types_;
  void **args_;
  QSemaphore *semaphore_;
  StaticMetaCallFunction callFunction_; // specific
  ushort method_offset_;      // specific
  ushort method_relative_;    // specific
};

#else

class QMetaCallEvent : public QEvent
{
public:
  QMetaCallEvent(int id,
                 const QObject *sender, int signalId,
                 int nargs = 0, int *types = 0, void **args = 0, QSemaphore *semaphore = 0);
  ~QMetaCallEvent();
  virtual int placeMetaCall(QObject *object);

public:
  const QObject *sender() const { return sender_; }
  int signalId() const { return signalId_; }
  int nargs() const { return nargs_; }
  int *types() const { return types_; }
  void **args() const { return args_; }

  int id() const { return id_; }

private:
  int id_; // specific
  const QObject *sender_;
  int signalId_;
  int nargs_;
  int *types_;
  void **args_;
  QSemaphore *semaphore_;
};

#endif // MCE_HAS_STATIC_CALL

QT_END_NAMESPACE

#endif // METACALLEVENT_H
