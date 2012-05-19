#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include "project/common/acapplication.h"
#include <QtGui/QWidgetList>

class Application : public AcApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(Application)
  typedef Application Self;
  typedef AcApplication Base;

  bool closed_;

  // - Constructions -
public:
  static Self *globalInstance() { return static_cast<Self *>(qApp); }

  Application(int &argc, char **argv);
  ~Application() { close(); }

  bool isClosed() const { return closed_; }

protected:
  void createDirectories();

public slots:
  void close();
};

#endif // APPLICATION_H
