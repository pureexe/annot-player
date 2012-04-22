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
  QList<QtMsgHandler> messageHandlers_;

  // - Constructions -
public:
  static Self *globalInstance() { return dynamic_cast<Self*>(qApp); }

  Application(int &argc, char **argv );
  ~Application();

public slots:
  void infectDownloadDirectory();
  void abortAll(); // kill all instances
  void abort();    // kill this instance

  // - Properties -
public:
  QWidget *mainWindow() const { return w_; }
  void setMainWindow(QWidget *w) { w_ = w; }

  bool isSingleInstance() const;


public:
  void installMessageHandlers(); ///< invoked only once

  void addMessageHandler(QtMsgHandler callback)
  { messageHandlers_.append(callback); }

  // - Events -
protected:
  virtual bool event(QEvent *e); ///< \override

  // - Implementation -
private:
  static void messageHandler(QtMsgType type, const char *msg);
  static void loggedMessageHandler(QtMsgType type, const char *msg);
  void createDirectories();
};

#endif // APPLICATION_H
