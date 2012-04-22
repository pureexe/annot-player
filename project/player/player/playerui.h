#ifndef PLAYERUI_H
#define PLAYERUI_H

// playerui.h
// 7/17/2011

#include "playerpanel.h"

QT_FORWARD_DECLARE_CLASS(QMenu)

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
  virtual void setVisible(bool visible); ///< \override
  bool isActive() const { return active_; }

  QString currentText() const;

signals:
  void invalidateMenuRequested();
  void textEntered(const QString &text);
  void loginRequested();
  void showPositionPanelRequested();
  void invalidateUserMenuRequested();

public slots:
  // - Player -
  void play();
  void stop();
  void nextFrame();
  void fastForward(qreal reate);
  void fastForward();
  void fastFastForward();
  void rewind();

  //void setAnnotationEnabled(bool enabled); // invalidateToggleAnnotationButton

  void setVolume(int vol);
  void setPosition(int pos);
  virtual void setTitle(const QString &title);  // TODO: virtual is used by playerui, BAD DESIGN STYLE!!!

  void setActive(bool active); // start polling whe active

  void setProgressMessage(const QString &text);
  void clearProgressMessage() { setProgressMessage(QString()); }

  void invalidateTitle();
  void invalidateVolumeSlider();
  void invalidatePositionSlider();

  void invalidateUserButton();
  void invalidatePositionButton();
  void invalidateProgressButton();

  void invalidatePlayButton();
  void invalidateStopButton();
  void invalidateNextFrameButton();
  void invalidateFastForwardButton();
  void invalidateFastFastForwardButton();
  //void invalidateRewindButton();
  void invalidateNextButton();
  void invalidatePreviousButton();
  virtual void invalidateInputCountButton();

  void invalidatePlayerModeToggler();
  void invalidateWindowModeToggler();

  void clickUserButton();

  void invalidateVisibleWidgets();

  // - Comments -
  void postAnnotation(); ///< Post annotation in lineEdit()

protected slots:
  void popupMenu();
  void popupInputItems();

private:
  void createConnections();
};

#endif // PLAYERUI_H
