#ifndef MAINOBJECT_P_H
#define MAINOBJECT_P_H

// mainobject.h
// 9/26/2012 jichi

#include "qx/qxglobal.h"
#include <QtCore/QObject>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWidget)
QT_FORWARD_DECLARE_CLASS(QWindow)

class MainObjectPrivate : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MainObjectPrivate)
  QX_EXTEND_CLASS(MainObjectPrivate, QObject)

public:
  explicit MainObjectPrivate(QObject *parent = nullptr);
  ~MainObjectPrivate();

  QWidget *rootWidget();

  QWindow *libraryDialog();

signals:
  void quitRequested();

private:
  QWidget *rootWidget_;
  QList<QWindow *> windows_;
  QWindow *libraryDialog_;
};

#endif // MAINOBJECT_P_H
