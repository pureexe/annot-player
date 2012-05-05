#ifndef MAINPLAYER_H
#define MAINPLAYER_H

// mainplayer.h
// 6/30/2011

#include "playerui.h"
#include <QtGui/QDockWidget>

class MainPlayerDock : public QDockWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(MainPlayerDock)
  typedef MainPlayerDock Self;
  typedef QDockWidget Base;

public:
  explicit MainPlayerDock(QWidget *parent = 0);

public:
  virtual void setVisible(bool visible);
};

class MainPlayerUi : public PlayerUi
{
  Q_OBJECT
  Q_DISABLE_COPY(MainPlayerUi)
  typedef MainPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit MainPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

private:
  void createLayout();
};

#endif // MAINPLAYER_H
