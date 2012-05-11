#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include "project/common/acapplication.h"

class Application : public AcApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(Application)
  typedef Application Self;
  typedef AcApplication Base;

  // - Constructions -
public:
  Application(int &argc, char **argv);
  ~Application();

protected:
  void createDirectories();
};

#endif // APPLICATION_H
