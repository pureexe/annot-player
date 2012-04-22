#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include <QtGui/QApplication>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWidget)

class Application : public QApplication
{
  Q_OBJECT
  typedef Application Self;
  typedef QApplication Base;

  QWidget *w_;

  // - Constructions -
public:
  static Self *globalInstance() { return dynamic_cast<Self*>(qApp); }

  Application(int &argc, char **argv, bool gui = true);
  ~Application();

protected:
  void createDirectories();

public slots:
  void abortAll(); // kill all instances
  void abort();    // kill this instance

  // - Properties -
public:
  QWidget *mainWindow() const { return w_; }
  void setMainWindow(QWidget *w) { w_ = w; }

  bool isSingleInstance() const;

  // - Events -
protected:
  virtual bool event(QEvent *e); ///< \override
};

#endif // APPLICATION_H
