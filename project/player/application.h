#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include <QApplication>

QT_FORWARD_DECLARE_CLASS(QWidget)

class Application : public QApplication
{
  Q_OBJECT
  typedef Application Self;
  typedef QApplication Base;

  QWidget *w_;

  // - Constructions -
public:
  Application(int &argc, char **argv );

  void startLoggingDebugMessage(); // invoke only once

  // - Properties -
  QWidget *mainWindow() const { return w_; }
  void setMainWindow(QWidget *w) { w_ = w; }

  bool isSingleInstance() const;

  // - Events -
protected:
  virtual bool event(QEvent *e); ///< \override

  // - Implementation -
private:
  void createDirectories();
};

#endif // APPLICATION_H
