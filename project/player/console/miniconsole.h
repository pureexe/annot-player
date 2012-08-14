#ifndef MINICONSOLE_H
#define MINICONSOLE_H

// miniconsole.h
// 7/18/2012

#include <QtCore/QMutex>
#include <QtCore/QStringList>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTimer;
QT_END_NAMESPACE

class MiniConsole : public QLabel
{
  Q_OBJECT
  Q_DISABLE_COPY(MiniConsole)
  typedef MiniConsole Self;
  typedef QLabel Base;

  QMutex mutex_;
  QTimer *timer_;
  QPoint dragPos_;

  QSize areaSize_;

public:
  explicit MiniConsole(QWidget *parent = nullptr);

  void setAutoClearInterval(int msecs);

  bool isEmpty() { return text().isEmpty(); }

signals:
  void asyncSetText(const QString &text); ///< \internal

  // - Output -
public:
  bool isAutoClearTimerActive() const;
public slots:
  void resize(const QSize &sz) { Base::resize(sz); }
  void sendMessage(const QString &text);

signals:
  void restartAutoClearTimerRequested();

public slots:
  ///  Restart timer for automatic clear the text after specified interval.
  void restartAutoClearTimer();
  void stopAutoClearTimer();

private:
  //void createActions();
  //void createMenus();
  //QMenu *contextMenu_;
  //QAction *enableAutoClearAct_;
  //QAction *disableAutoClearAct_;
};

#endif // MINICONSOLE_H
