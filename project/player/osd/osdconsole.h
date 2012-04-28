#ifndef OSDCONSOLE_H
#define OSDCONSOLE_H

// osdconsole.h
// 7/11/2011

#include <QtGui/QLabel>
#include <QtCore/QMutex>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTimer;
QT_END_NAMESPACE

class OsdConsole : public QLabel
{
  Q_OBJECT
  typedef OsdConsole Self;
  typedef QLabel Base;

  QMutex mutex_;
  QTimer *timer_;
  QPoint dragPos_;

  static Self *global_;
public:
  static Self *globalInstance() { return global_; } ///< default is 0.
  static void setGlobalInstance(Self *global) { global_ = global; }

public:
  explicit OsdConsole(QWidget *parent = 0);

  void setAutoClearInterval(int msecs);

  bool isEmpty() { return text().isEmpty(); }

signals:
  void asyncSetText(const QString &text); ///< \internal

  // - Output -
public:
  void append(const QString &text);
  Self &operator<<(const QString &text);

  bool isAutoClearTimerActive() const;

signals:
  void restartAutoClearTimerRequested();

public slots:
  ///  Restart timer for automatic clear the text after specified interval.
  void restartAutoClearTimer();
  void stopAutoClearTimer();

  // Events:
protected:
  /*
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void contextMenuEvent(QContextMenuEvent *event);
  */

private:
  void createActions();
  void createMenus();

  QMenu *contextMenu_;
  QAction *enableAutoClearAct_;
  QAction *disableAutoClearAct_;

};

// Default global console. Global instance must be set before invoke this function.
inline OsdConsole&
gConsole()
{
  OsdConsole *g = OsdConsole::globalInstance();
  Q_ASSERT(g);
  return (*g);
}

#endif // OSDCONSOLE_H
