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
  explicit MainPlayerDock(QWidget *parent = nullptr);

public:
  void setVisible(bool visible) override;
};

class MainPlayerUi : public PlayerUi
{
  Q_OBJECT
  Q_DISABLE_COPY(MainPlayerUi)
  typedef MainPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit MainPlayerUi(SignalHub *hub, Player *player, AnnotationServerAgent *server, QWidget *parent = nullptr);

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private:
  void createLayout();
};

#endif // MAINPLAYER_H
