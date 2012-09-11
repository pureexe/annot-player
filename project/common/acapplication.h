#ifndef ACAPPLICATION_H
#define ACAPPLICATION_H

// acapplication.h
// 11/18/2011

#include <QtGui/QApplication>
#include <QtGui/QWidgetList>

#ifdef Q_OS_WIN
struct _EXCEPTION_POINTERS;
#endif // Q_OS_WIN

class AcApplication : public QApplication
{
  Q_OBJECT
  Q_DISABLE_COPY(AcApplication)
  typedef AcApplication Self;
  typedef QApplication Base;

  static QString applicationFilePath_; // cached, invariant on Windows and Linux
  QList<QtMsgHandler> messageHandlers_;
  QString logFileName_,
          lockFileName_;
  QWidgetList windows_;

  // - Constructions -
public:
  static Self *globalInstance() { return static_cast<Self *>(qApp); }

  AcApplication(int &argc, char **argv)
    : Base(argc, argv) { init(); }
  AcApplication(int &argc, char **argv, bool gui)
    : Base(argc, argv, gui) { init(); }

  ~AcApplication();

public slots:
  void abortAll(); // kill all instances
  void abort();    // kill this instance

  // - States -
public:
  bool isSingleInstance() const;

  // - Logs -
public:
  void setLogFileName(const QString &path) { logFileName_ = path; }
  void setLockFileName(const QString &path) { lockFileName_ = path; }

  void installMessageHandlers(); ///< invoked only once
  void addMessageHandler(QtMsgHandler callback)
  { messageHandlers_.append(callback); }

  // - Windows -
public:
  QWidget *mainWindow() const;
  void setMainWindow(QWidget *w) { addWindow(w); }
  void addWindow(QWidget *w);
  void removeWindow(QWidget *w);

  // - i18n -
public:
  void loadTranslations();

  // - Events -
public:
  bool event(QEvent *e) override;

  // - Implementation -
protected:
#ifdef Q_OS_WIN
  void ignoreWindowsExcetions();

  // static LONG WINAPI exceptionFilter(PEXCEPTION_POINTERS e)
  static long __stdcall exceptionFilter(_EXCEPTION_POINTERS *e);
#else
  void ignoreWindowsExcetions() { }
#endif // Q_OS_WIN

private:
  void init();
  static void messageHandler(QtMsgType type, const char *msg);
  static void loggedMessageHandler(QtMsgType type, const char *msg);
};

#endif // ACAPPLICATION_H
