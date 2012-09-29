#ifndef APPLICATION_H
#define APPLICATION_H

// application.h
// 9/22/2012 jichi

#include "qx/qxglobal.h"
#include <QtWidgets/QApplication>

class Application : public QApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(Application)
  QX_EXTEND_CLASS(Application, QApplication)

public:
  Application(int &argc, char **argv);
};

#endif // APPLICATION_H
