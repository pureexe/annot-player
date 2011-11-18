#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include <QApplication>

class MainWindow;

class Application : public QApplication
{
  Q_OBJECT
  typedef Application Self;
  typedef QApplication Base;

  MainWindow *w_;

  // - Constructions -
public:
  Application(int &argc, char **argv );

  MainWindow *mainWindow() const;
  void setMainWindow(MainWindow *w);

  // - Events -
protected:
  virtual bool event(QEvent *e); ///< \override
};

#endif // APPLICATION_H
