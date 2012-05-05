#ifndef APPLICATION_H
#define APPLICATION_H
// application.h
// 11/18/2011

#include <QtGui/QApplication>
#include <QtGui/QWidgetList>
#include <QtCore/QList>

class Application : public QApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(Application)
  typedef Application Self;
  typedef QApplication Base;

  bool closed_;
  QWidgetList windows_;
  QList<QtMsgHandler> messageHandlers_;

  // - Constructions -
public:
  static Self *globalInstance() { return dynamic_cast<Self *>(qApp); }

  Application(int &argc, char **argv, bool gui = true);
  ~Application() { close(); }

  bool isClosed() const { return closed_; }

protected:
  void createDirectories();

public slots:
  void abortAll(); // kill all instances
  void abort();    // kill this instance
  void close();

  // - Properties -
public:
  QWidget *activeMainWindow() const;
  void setActiveMainWindow(QWidget *w) { addWindow(w); }
  void addWindow(QWidget *w);
  void removeWindow(QWidget *w);

  bool isSingleInstance() const;

  // - Debug -
public:
  void installMessageHandlers(); ///< invoked only once

  void addMessageHandler(QtMsgHandler callback)
  { messageHandlers_.append(callback); }

private:
  static void messageHandler(QtMsgType type, const char *msg);
  static void loggedMessageHandler(QtMsgType type, const char *msg);

  // - Events -
protected:
  virtual bool event(QEvent *e); ///< \override
};

#endif // APPLICATION_H
