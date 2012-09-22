#ifndef APPLICATION_H
#define APPLICATION_H

// application.h
// 11/18/2011

#include "src/common/acapplication.h"

class Application : public AcApplication
{
  Q_OBJECT
  //Q_PROPERTY(Qt::CursorShape cursor READ cursor WRITE setCursor)
  Q_DISABLE_COPY(Application)
  typedef Application Self;
  typedef AcApplication Base;

  // - Constructions -
public:
  static Self *globalInstance() { return static_cast<Self *>(qApp); }

  Application(int &argc, char **argv);
  ~Application();

  // - Implementation -
private:
  void createDirectories();
};

#endif // APPLICATION_H
