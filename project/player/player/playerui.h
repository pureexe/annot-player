#ifndef PLAYERUI_H
#define PLAYERUI_H

// playerui.h
// 7/17/2011

#include "playerpanel.h"

class SignalHub;
class Player;
class ServerAgent;

 class PlayerUi : public PlayerPanel
{
  Q_OBJECT
  typedef PlayerUi Self;
  typedef PlayerPanel Base;

  SignalHub *hub_;
  Player *player_;
  ServerAgent *server_;
  bool active_;

  // TODO: use signal hub_ to replace player_ and server_ signals.
public:
  explicit PlayerUi(SignalHub *hub_, Player *player, ServerAgent *server, QWidget *parent = 0);

  bool isValid() const;

protected:
  SignalHub *hub() const;
  void connectHub();
  void disconnectHub();

  Player *player() const;
  void connectPlayer();
  void disconnectPlayer();

  ServerAgent *server() const;
  void connectServer();
  void disconnectServer();

public:
  ///  Override
  virtual void setVisible(bool visible); // stop polling when hidden
  bool isActive() const; ///< This status is used to track if media player is connected

signals:
  void commandEntered(const QString &command);
  void showUserPanelRequested();
  void showPositionPanelRequested();

public slots:
  // - Player -
  void play();
  void stop();
  void nextFrame();

  void setAnnotationEnabled(bool enabled); // invalidateToggleAnnotationButton

  void setVolume(int vol);
  void setPosition(int pos);
  virtual void setTitle(const QString &title);  // TODO: virtual is used by playerui, BAD DESIGN STYLE!!!

  void setActive(bool active); // start polling whe active

  void invalidateTitle();
  void invalidateVolumeSlider();
  void invalidatePositionSlider();

  void invalidateUserButton();
  void invalidatePositionButton();

  void invalidatePlayButton();
  void invalidateStopButton();
  void invalidateNextFrameButton();

  void invalidatePlayerModeToggler();
  void invalidateVideoModeToggler();

  void requestShowUserPanel();
  void requestShowPositionPanel();

  // - Comments -
  void postAnnotation(); ///< Post annotation in lineEdit()

private:
  void createConnections();
};

#endif // PLAYERUI_H
