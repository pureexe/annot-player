#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

// mainconsole.h
// 7/11/2011

#include <QtGui/QLabel>
#include <QtCore/QMutex>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTimer;
QT_END_NAMESPACE

class MainConsole : public QLabel
{
  Q_OBJECT
  Q_DISABLE_COPY(MainConsole)
  typedef MainConsole Self;
  typedef QLabel Base;

  QMutex mutex_;
  QTimer *timer_;
  QPoint dragPos_;
  QStringList messages_;

  QSize areaSize_;
  QString text_;

public:
  explicit MainConsole(QWidget *parent = nullptr);

  void setAutoClearInterval(int msecs);

  bool isEmpty() { return text_.isEmpty(); }

signals:
  void asyncSetText(const QString &text); ///< \internal
  void asyncUpdateText(); ///< \internal
  void restartAutoClearTimerRequested(); ///< \internal

  void message(const QString &text);

  // - Output -
public:
  bool isAutoClearTimerActive() const;
  //QString text() const { return text_; }

public slots:
  void resize(const QSize &sz) { Base::resize(sz); }
  void sendMessage(const QString &text);

  void setText(const QString &text);

  void clearAll();
  void setAreaSize(const QSize &sz) { areaSize_= sz; }

  ///  Restart timer for automatic clear the text after specified interval.
  void restartAutoClearTimer();
  void stopAutoClearTimer();

protected slots:
  void updateText();
  void updateFormat();

protected:
  bool isAreaFull() const;

private:
  //void createActions();
  //void createMenus();
  //QMenu *contextMenu_;
  //QAction *enableAutoClearAct_;
  //QAction *disableAutoClearAct_;
};

#endif // MAINCONSOLE_H
