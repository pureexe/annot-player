#ifndef MAINOBJECT_H
#define MAINOBJECT_H

// mainobject.h
// 9/26/2012 jichi

#include "qx/qxglobal.h"
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QStringList)

class MainObjectPrivate;
class MainObject : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MainObject)
  QX_EXTEND_CLASS(MainObject, QObject)
  QX_DECLARE_PRIVATE(MainObjectPrivate)

public:
  MainObject();
  ~MainObject();

  // Launch with given arguments
  void launch(const QStringList &args);

  // Quit the application
public slots:
  void quit();
};

#endif // MAINOBJECT_H
