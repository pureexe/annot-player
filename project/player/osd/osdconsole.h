#ifndef OSDCONSOLE_H
#define OSDCONSOLE_H

// osdconsole.h
// 7/11/2011

#include <QLabel>

QT_BEGIN_NAMESPACE
class QTimer;
class QMenu;
class QAction;
QT_END_NAMESPACE

//
class OSDConsole : public QLabel
{
  Q_OBJECT
  typedef OSDConsole Self;
  typedef QLabel Base;

  QTimer *timer_;
  QPoint dragPos_;

  static Self *global_;
public:
  static Self *globalInstance(); // default is 0.
  static void setGlobalInstance(Self *global);

public:
  explicit OSDConsole(QWidget *parent = 0);

  void setAutoClearInterval(int msecs);

  // - Output -
public:
  Self &operator<<(const QString &text);

  bool isAutoClearTimerActive() const;

public slots:
  ///  Restart timer for automatic clear the text after specified interval.
  void restartAutoClearTimer();
  void stopAutoClearTimer();
  void copyToClipboard() const;
  void pasteFromClipboard();

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
inline OSDConsole&
gConsole()
{
  OSDConsole *g = OSDConsole::globalInstance();
  Q_ASSERT(g);
  return (*g);
}

#endif // OSDCONSOLE_H
