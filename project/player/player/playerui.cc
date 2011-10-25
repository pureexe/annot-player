// playerui.cc
// 7/17/2011

#include "playerui.h"
#include "global.h"
#include "stylesheet.h"
#include "tr.h"
#include "commandlineedit.h"
#include "signalhub.h"
#include "module/player/player.h"
#include "module/serveragent/serveragent.h"
#include "core/util/datetime.h"
#include "core/data/annotation.h"
#include "boost/foreach.hpp"
#include <map>
#include <QtGui>

using namespace Core::Data;

/* jichi 7/25/2011: using boost is so bloated while less efficient than directly expanding the code.
#include <boost/typeof/typeof.hpp>
#include <boost/assign/list_of.hpp>
namespace { // anonymous
  // Player connections
  const QMap<const char*, const char*> &playerConnections_()
  {
    static QMap<const char*, const char*> connections
      (std::map<const char*, const char*>(boost::assign::map_list_of
        (SIGNAL(mediaChanged()), SLOT(invalidateTitle()))
        (SIGNAL(mediaChanged()), SLOT(invalidatePositionSlider()))
        (SIGNAL(mediaChanged()), SLOT(invalidatePositionButton()))
        (SIGNAL(encodingChanged()), SLOT(invalidateTitle()))
        (SIGNAL(lengthChanged()), SLOT(invalidatePositionButton()))
        (SIGNAL(timeChanged()), SLOT(invalidatePositionButton()))
        (SIGNAL(positionChanged()), SLOT(invalidatePositionSlider()))
        (SIGNAL(volumeChanged()), SLOT(invalidateVolumeSlider()))
        (SIGNAL(playing()), SLOT(invalidatePlayButton()))
        (SIGNAL(paused()), SLOT(invalidatePlayButton()))
        (SIGNAL(stopped()), SLOT(invalidatePlayButton()))
        (SIGNAL(playing()), SLOT(invalidateStopButton()))
        (SIGNAL(paused()), SLOT(invalidateStopButton()))
        (SIGNAL(stopped()), SLOT(invalidateStopButton()))
        (SIGNAL(playing()), SLOT(invalidateNextFrameButton()))
        (SIGNAL(paused()), SLOT(invalidateNextFrameButton()))
        (SIGNAL(stopped()), SLOT(invalidateNextFrameButton()))
      ));

    return connections;
  }

} // anonymous namespace

void
PlayerUi::connectPlayer()
{
  for (BOOST_AUTO(i, ::playerConnections_().begin()); i != ::playerConnections_().end(); ++i)
    connect(player_, i.key(), i.value());
}

void
PlayerUi::disconnectPlayer()
{
  for (BOOST_AUTO(i, ::playerConnections_().begin()); i != ::playerConnections_().end(); ++i)
    disconnect(player_, i.key(), i.value());
}
*/

 // - Constructions -

PlayerUi::PlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent)
  : Base(parent), hub_(hub), player_(player), server_(server), active_(false)
{
  Q_ASSERT(isValid());

  // Post fix: remove blurred graphics effects for text buttons
  userButton()->setGraphicsEffect(0);

  toggleMiniModeButton()->setCheckable(true);
  togglePlayModeButton()->setCheckable(true);

  setTabOrder(lineEdit(), prefixLineEdit());
  setTabOrder(prefixLineEdit(), lineEdit());

  createConnections();

  lineEdit()->setFocus();
}

void
PlayerUi::createConnections()
{
  connect(playButton(), SIGNAL(clicked()), SLOT(play()));
  connect(stopButton(), SIGNAL(clicked()), SLOT(stop()));
  connect(nextFrameButton(), SIGNAL(clicked()), SLOT(nextFrame()));

  connect(userButton(), SIGNAL(clicked()), SLOT(requestShowUserPanel()));
  connect(positionButton(), SIGNAL(clicked()), SLOT(requestShowPositionPanel()));

  connect(positionSlider(), SIGNAL(valueChanged(int)), SLOT(setPosition(int)));
  connect(volumeSlider(), SIGNAL(valueChanged(int)), SLOT(setVolume(int)));

  connect(lineEdit(), SIGNAL(returnPressed()), SLOT(postAnnotation()));

  CommandLineEdit *p = dynamic_cast<CommandLineEdit*>(prefixLineEdit());
  if (p)
    connect(lineEdit(), SIGNAL(returnPressed()), p, SLOT(addCurrentTextToHistory()));
}

#define CREATE_HUB_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Hub() \
  { \
    connect(openButton(), SIGNAL(clicked()), hub_, SLOT(open())); \
    connect(togglePlayModeButton(), SIGNAL(clicked(bool)), hub_, SLOT(setFullScreenVideoMode(bool))); \
    connect(toggleMiniModeButton(), SIGNAL(clicked(bool)), hub_, SLOT(setMiniPlayerMode(bool))); \
    connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(invalidatePlayButton())); \
    connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(invalidateStopButton())); \
    connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(invalidatePositionSlider())); \
\
    static const char *status_signals[] = { \
      SIGNAL(played()), SIGNAL(paused()), SIGNAL(stopped()) \
    }; \
    static const char *invalidate_slots[] = { \
      SLOT(invalidatePlayButton()), \
      SLOT(invalidateStopButton()), \
    }; \
    BOOST_FOREACH (const char *signal, status_signals) \
      BOOST_FOREACH (const char *slot, invalidate_slots) \
        _connect(hub_, signal, this, slot); \
  }

  CREATE_HUB_CONNECTIONS(connect)
  CREATE_HUB_CONNECTIONS(disconnect)
#undef CREATE_HUB_CONNECTIONS

#define CREATE_PLAYER_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Player() \
  { \
    _connect(player_, SIGNAL(encodingChanged()), this, SLOT(invalidateTitle())); \
\
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(invalidateTitle())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(invalidateTitle())); \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(invalidatePositionSlider())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(invalidatePositionSlider())); \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(invalidatePositionButton())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(invalidatePositionButton())); \
\
    _connect(player_, SIGNAL(lengthChanged()), this, SLOT(invalidatePositionButton())); \
    _connect(player_, SIGNAL(timeChanged()), this, SLOT(invalidatePositionButton())); \
    _connect(player_, SIGNAL(positionChanged()), this, SLOT(invalidatePositionSlider())); \
    _connect(player_, SIGNAL(volumeChanged()), this, SLOT(invalidateVolumeSlider())); \
 \
    static const char *status_signals[] = { \
      SIGNAL(playing()), SIGNAL(paused()), SIGNAL(stopped()) \
    }; \
    static const char *invalidate_slots[] = { \
      SLOT(invalidatePlayButton()), \
      SLOT(invalidateStopButton()), \
      SLOT(invalidateNextFrameButton()) \
    }; \
    BOOST_FOREACH (const char *signal, status_signals) \
      BOOST_FOREACH (const char *slot, invalidate_slots) \
        _connect(player_, signal, this, slot); \
  }

  CREATE_PLAYER_CONNECTIONS(connect)
  CREATE_PLAYER_CONNECTIONS(disconnect)
#undef CREATE_PLAYER_CONNECTIONS

#define CREATE_SERVER_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Server() \
  { \
    _connect(server_, SIGNAL(userChanged()), this, SLOT(invalidateUserButton())); \
  }

  CREATE_SERVER_CONNECTIONS(connect)
  CREATE_SERVER_CONNECTIONS(disconnect)
#undef CREATE_SERVER_CONNECTIONS

bool
PlayerUi::isValid() const
{ return hub_ && player_ && player_->isValid() && server_; }

void
PlayerUi::setTitle(const QString &title)
{ Base::setWindowTitle(title); }

void
PlayerUi::setVisible(bool visible)
{
  setActive(visible);
  Base::setVisible(visible);
}

bool
PlayerUi::isActive() const
{ return active_; }

void
PlayerUi::setActive(bool active)
{
  if (active) {
    invalidateTitle();
    invalidateVolumeSlider();
    invalidatePositionSlider();
    invalidatePositionButton();
    invalidatePlayButton();
    invalidateStopButton();
    invalidateNextFrameButton();
    invalidatePlayerModeToggler();
    invalidateVideoModeToggler();

    invalidateUserButton();

    if (!active_) {
      connectHub();
      connectPlayer();
      connectServer();
      active_ = true;
    }
  } else if (active_) {
    disconnectHub();
    disconnectPlayer();
    disconnectServer();
    active_ = false;
  }
}

// - Play control -

void
PlayerUi::play()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying())
      hub_->pause();
    else
      hub_->play();
    break;

  case SignalHub::MediaTokenMode:
    if (!player_->hasMedia())
      hub_->open();
    else
      switch (player_->status()) {
      case Player::Playing:
        player_->pause();
        break;
      case Player::Paused:
        player_->resume();
        break;
      case Player::Stopped:
        player_->play();
        break;
      }
    break;
  }
}

void
PlayerUi::stop()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    hub_->stop();
    break;

  case SignalHub::MediaTokenMode:
    if (player_->hasMedia())
      switch (player_->status()) {
      case Player::Playing:
      case Player::Paused:
        player_->stop();
        break;

      case Player::Stopped:
        break;
      }
    break;
  }
}

void
PlayerUi::nextFrame()
{
  if (hub_->isMediaTokenMode())
    if (player_->hasMedia())
      player_->nextFrame();
}

// - Set/get properties -

SignalHub*
PlayerUi::hub() const
{ Q_ASSERT(hub_); return hub_; }

Player*
PlayerUi::player() const
{ Q_ASSERT(player_); return player_; }

ServerAgent*
PlayerUi::server() const
{ Q_ASSERT(server_); return server_; }

void
PlayerUi::setVolume(int vol)
{ player_->setVolume(vol / G_VOLUME_MAX); }

void
PlayerUi::setPosition(int pos)
{ player_->setPosition(pos / G_POSITION_MAX); }

void
PlayerUi::invalidateVolumeSlider()
{
  qreal vol = player_->volume();
  volumeSlider()->setSliderPosition(
        static_cast<int>(vol * G_VOLUME_MAX)
        );

  // Update tool tip.
  int percentage = qRound(vol * 100);
  volumeSlider()->setToolTip(
        TR(T_VOLUME) + " " + QString::number(percentage) + "%");
}

void
PlayerUi::invalidatePositionSlider()
{
  QSlider *slider = positionSlider();

  if (hub_->isSignalTokenMode()) {
    //slider->hide();
    slider->setEnabled(false);
    return;
  }

  //slider->show();

  if (player_->hasMedia()) {
    if (!slider->isEnabled())
      slider->setEnabled(true);

    if (!slider->isSliderDown()) {
      qreal  pos = player_->position();
      slider->setSliderPosition(
            static_cast<int>(pos * G_POSITION_MAX)
            );
    }

    // Update slider's tool tip and label's text.
    qint64 current_msecs = player_->time(),
           total_msecs = player_->mediaLength();
    qint64 left_msecs = total_msecs - current_msecs;

    QTime current = Core::msecs2time(current_msecs),
          left = Core::msecs2time(left_msecs);

    slider->setToolTip(current.toString() + " / -" + left.toString());

  } else {
    if (slider->isEnabled())
      slider->setEnabled(false);

    QString zero = QTime(0, 0, 0).toString();
    slider->setToolTip(zero + " / -" + zero);
  }
}

void
PlayerUi::invalidateUserButton()
{
  if (server_->isAuthorized()) {
    userButton()->setStyleSheet(SS_TOOLBUTTON_TEXT_NORMAL);
    userButton()->setText(server_->user().name());
  } else {
    userButton()->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);
    userButton()->setText(TR(T_LOGIN));
  }
}


void
PlayerUi::invalidatePositionButton()
{
  QToolButton *button = positionButton();

  if (player_->hasMedia()) {
    if (!button->isEnabled())
      button->setEnabled(true);

    // Update slider's tool tip and button's text.
    qint64 current_msecs = player_->time(),
           total_msecs = player_->mediaLength();
    //qint64 left_msecs = total_msecs - current_msecs;
    //QTime left = Core::msecs2time(left_msecs),

    QTime current = Core::msecs2time(current_msecs),
          total = Core::msecs2time(total_msecs);

    button->setText(current.toString() + " / " + total.toString());

  } else {
    if (button->isEnabled())
      button->setEnabled(false);

    QString zero = QTime(0, 0, 0).toString();
    button->setText(zero + " / " + zero);
  }
}

void
PlayerUi::invalidateTitle()
{
  if (hub_->isSignalTokenMode())
    return;
  if (player_->hasMedia()) {
    QString title = player_->mediaTitle();
    if (title.isEmpty()) {
      title = player_->mediaPath();
      if (!title.isEmpty())
        title = QFileInfo(title).fileName();
    }
    if (title.isEmpty())
      title = TR(T_TITLE_PROGRAM);

    setTitle(title);
  } else
    setTitle(TR(T_TITLE_PROGRAM));
}

void
PlayerUi::invalidatePlayButton()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying()) {
      playButton()->setStyleSheet(SS_TOOLBUTTON_PAUSE);
      playButton()->setToolTip(TR(T_TOOLTIP_PAUSE));
    } else {
      playButton()->setStyleSheet(SS_TOOLBUTTON_PLAY);
      playButton()->setToolTip(TR(T_TOOLTIP_PLAY));
    }
    break;

  case SignalHub::MediaTokenMode:
    switch (player_->status()) {
    case Player::Playing:
      playButton()->setStyleSheet(SS_TOOLBUTTON_PAUSE);
      playButton()->setToolTip(TR(T_TOOLTIP_PAUSE));
      break;
    case Player::Stopped:
    case Player::Paused:
      playButton()->setStyleSheet(SS_TOOLBUTTON_PLAY);
      playButton()->setToolTip(TR(T_TOOLTIP_PLAY));
      break;
    }
    break;
  }
}

void
PlayerUi::invalidateStopButton()
{
  switch (hub_->tokenMode()) {
  case SignalHub::SignalTokenMode:
    stopButton()->setEnabled(!hub_->isStopped());
    break;

  case SignalHub::MediaTokenMode:
    switch (player_->status()) {
    case Player::Playing:
    case Player::Paused:
      stopButton()->setEnabled(true);
      break;
    case Player::Stopped:
      stopButton()->setEnabled(false);
      break;
    }
    break;
  }
}

void
PlayerUi::invalidateNextFrameButton()
{
  QToolButton *button = nextFrameButton();
  if (hub_->isSignalTokenMode()) {
    button->hide();
    button->setEnabled(false);
    return;
  }

  button->show();

  switch (player_->status()) {
  case Player::Playing:
  case Player::Paused:
    button->setEnabled(true);
    break;
  case Player::Stopped:
    button->setEnabled(false);
    break;
  }
}

// - Annotations -

void
PlayerUi::postAnnotation()
{
  QString text = lineEdit()->text();
  if (text.isEmpty())
    return;

  lineEdit()->clear();

  QString prefix = prefixLineEdit()->text();
  if (!prefix.trimmed().isEmpty())
    text = prefix + " " + text;

  // TODO: use command filter rather than annnotation filter here
  if (Annotation::isValidText(text))
    emit commandEntered(text);
}

// - Requests -

void
PlayerUi::requestShowUserPanel()
{ emit showUserPanelRequested(); }

void
PlayerUi::requestShowPositionPanel()
{ emit showPositionPanelRequested(); }

void
PlayerUi::setAnnotationEnabled(bool enabled)
{
  if (enabled) {
    toggleAnnotationButton()->setStyleSheet(SS_TOOLBUTTON_HIDEANNOT);
    toggleAnnotationButton()->setToolTip(TR(T_TOOLTIP_HIDEANNOT));
  } else {
    toggleAnnotationButton()->setStyleSheet(SS_TOOLBUTTON_SHOWANNOT);
    toggleAnnotationButton()->setToolTip(TR(T_TOOLTIP_SHOWANNOT));
  }
}

void
PlayerUi::invalidatePlayerModeToggler()
{ toggleMiniModeButton()->setChecked(hub_->isMiniPlayerMode()); }

void
PlayerUi::invalidateVideoModeToggler()
{ togglePlayModeButton()->setChecked(hub_->isFullScreenVideoMode()); }

// EOF
