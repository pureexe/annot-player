#ifndef PLAYERUI_H
#define PLAYERUI_H

// playerui.h
// 7/17/2011

#include "playerpanel.h"

QT_FORWARD_DECLARE_CLASS(QMenu)

class Player;
class SignalHub;
class ServerAgent;

class PlayerUi : public PlayerPanel
{
  Q_OBJECT
  Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity)
  Q_DISABLE_COPY(PlayerUi)
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
  void updateMenuRequested();
  void textEntered(const QString &text);
  void loginRequested();
  void showPositionPanelRequested();
  void updateUserMenuRequested();

public slots:
  // - Player -
  void play();
  void stop();
  void nextFrame();
  void fastForward(qreal reate);
  void fastForward();
  void fastFastForward();
  void rewind();

  //void setAnnotationEnabled(bool enabled); // updateToggleAnnotationButton

  void setVolume(int vol);
  void setPosition(int pos);
  virtual void setTitle(const QString &title);  // TODO: virtual is used by playerui, BAD DESIGN STYLE!!!

  void setActive(bool active); // start polling whe active

  void setProgressMessage(const QString &text);
  void clearProgressMessage() { setProgressMessage(QString()); }

  void updateTitle();
  void updateVolumeSlider();
  void updatePositionSlider();

  void updateUserButton();
  void updatePositionButton();
  void updateProgressButton();
  void updateTraceWindowButton();

  void updatePlayButton();
  void updateStopButton();
  void updateNextFrameButton();
  void updateFastForwardButton();
  void updateFastFastForwardButton();
  //void updateRewindButton();
  void updateNextButton();
  void updatePreviousButton();
  virtual void updateInputCountButton();

  void updatePlayerModeToggler();
  void updateWindowModeToggler();

  void clickUserButton();

  void updateVisibleWidgets();

  // - Comments -
  void postAnnotation(); ///< Post annotation in lineEdit()

protected slots:
  void popupMenu();
  void popupInputItems();

private:
  void createConnections();
};

#endif // PLAYERUI_H
