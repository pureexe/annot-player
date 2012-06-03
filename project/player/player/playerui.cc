// playerui.cc
// 7/17/2011

#include "playerui.h"
#include "global.h"
#include "stylesheet.h"
#include "tr.h"
#include "signalhub.h"
#include "positionslider.h"
#include "module/player/player.h"
#include "module/serveragent/serveragent.h"
#include "module/qtext/datetime.h"
#include "module/annotcloud/annotation.h"
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include "boost/foreach.hpp"
#include <map>
#include <QtCore>
#include <QtGui>

using namespace AnnotCloud;

#ifdef Q_OS_MAC
#  define K_CTRL        "cmd"
#  define K_SHIFT       "shift"
#else
#  define K_CTRL        "Ctrl"
#  define K_SHIFT       "Shift"
#endif // Q_OS_MAC

/* jichi 7/25/2011: using boost is so bloated while less efficient than directly expanding the code.
#include <boost/typeof/typeof.hpp>
#include <boost/assign/list_of.hpp>
namespace { // anonymous
  // Player connections
  const QMap<const char*, const char*> &playerConnections_()
  {
    static QMap<const char*, const char*> connections
      (std::map<const char*, const char*>(boost::assign::map_list_of
        (SIGNAL(mediaChanged()), SLOT(updateTitle()))
        (SIGNAL(mediaChanged()), SLOT(updatePositionSlider()))
        (SIGNAL(mediaChanged()), SLOT(updatePositionButton()))
        (SIGNAL(encodingChanged()), SLOT(updateTitle()))
        (SIGNAL(lengthChanged()), SLOT(updatePositionButton()))
        (SIGNAL(timeChanged()), SLOT(updatePositionButton()))
        (SIGNAL(positionChanged()), SLOT(updatePositionSlider()))
        (SIGNAL(volumeChanged()), SLOT(updateVolumeSlider()))
        (SIGNAL(playing()), SLOT(updatePlayButton()))
        (SIGNAL(paused()), SLOT(updatePlayButton()))
        (SIGNAL(stopped()), SLOT(updatePlayButton()))
        (SIGNAL(playing()), SLOT(updateStopButton()))
        (SIGNAL(paused()), SLOT(updateStopButton()))
        (SIGNAL(stopped()), SLOT(updateStopButton()))
        (SIGNAL(playing()), SLOT(updateNextFrameButton()))
        (SIGNAL(paused()), SLOT(updateNextFrameButton()))
        (SIGNAL(stopped()), SLOT(updateNextFrameButton()))
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

  positionButton()->setToolTip(positionButton()->toolTip()  + " [" K_CTRL "+F]");
  positionSlider()->setToolTip(positionSlider()->toolTip()  + " [" K_CTRL "+F]");

  //setTabOrder(inputComboBox(), prefixComboBox());
  //setTabOrder(prefixComboBox(), inputComboBox());

  createConnections();

  inputComboBox()->setFocus();
}

void
PlayerUi::createConnections()
{
  toggleEmbedModeButton()->setCheckable(true);
  toggleMiniModeButton()->setCheckable(true);
  toggleFullScreenModeButton()->setCheckable(true);
  toggleTraceWindowButton()->setCheckable(true);

  connect(menuButton(), SIGNAL(clicked()), SLOT(popupMenu()));
  connect(openButton(), SIGNAL(clicked()), openButton(), SLOT(showMenu()));
  connect(playButton(), SIGNAL(clicked()), SLOT(play()));
  connect(stopButton(), SIGNAL(clicked()), SLOT(stop()));
  connect(nextFrameButton(), SIGNAL(clicked()), SLOT(nextFrame()));
  connect(fastForwardButton(), SIGNAL(pressed()), SLOT(fastForward()));
  connect(fastFastForwardButton(), SIGNAL(pressed()), SLOT(fastFastForward()));
  //connect(rewindButton(), SIGNAL(pressed()), SLOT(rewind()));
  connect(fastForwardButton(), SIGNAL(released()), player_, SLOT(clearRate()));
  connect(fastFastForwardButton(), SIGNAL(released()), player_, SLOT(clearRate()));
  //connect(rewindButton(), SIGNAL(released()), player_, SLOT(clearRate()));

  connect(userButton(), SIGNAL(clicked()), SLOT(clickUserButton()));
  connect(positionButton(), SIGNAL(clicked()), SIGNAL(showPositionPanelRequested()));

  connect(positionSlider(), SIGNAL(valueChanged(int)), SLOT(setPosition(int)));
  connect(volumeSlider(), SIGNAL(valueChanged(int)), SLOT(setVolume(int)));

  connect(inputComboBox()->lineEdit(), SIGNAL(returnPressed()), SLOT(postAnnotation()));

  connect(inputComboBox(), SIGNAL(editTextChanged(QString)), SLOT(updateInputCountButton()));
  connect(prefixComboBox(), SIGNAL(editTextChanged(QString)), SLOT(updateInputCountButton()));

  connect(inputCountButton(), SIGNAL(clicked()), SLOT(popupInputItems()));

  // Always connected
  connect(hub_, SIGNAL(tokenModeChanged(SignalHub::TokenMode)), SLOT(updateVisibleWidgets()));

  //CommandLineEdit *p = dynamic_cast<CommandLineEdit*>(prefixLineEdit());
  //if (p)
  //  connect(lineEdit(), SIGNAL(returnPressed()), p, SLOT(addCurrentTextToHistory()));
}

//_connect(openButton(), SIGNAL(clicked()), hub_, SLOT(open()));
#define CREATE_HUB_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Hub() \
  { \
    _connect(toggleFullScreenModeButton(), SIGNAL(clicked(bool)), hub_, SLOT(setFullScreenWindowMode(bool))); \
    _connect(toggleMiniModeButton(), SIGNAL(clicked(bool)), hub_, SLOT(setMiniPlayerMode(bool))); \
    _connect(toggleEmbedModeButton(), SIGNAL(clicked(bool)), hub_, SLOT(setEmbeddedPlayerMode(bool))); \
    _connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(updatePlayButton())); \
    _connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(updateStopButton())); \
    _connect(hub_, SIGNAL(playModeChanged(SignalHub::PlayMode)), this, SLOT(updatePositionSlider())); \
    _connect(hub_, SIGNAL(volumeChanged(qreal)), this, SLOT(updateVolumeSlider())); \
\
    static const char *status_signals[] = { \
      SIGNAL(played()), SIGNAL(paused()), SIGNAL(stopped()) \
    }; \
    static const char *update_slots[] = { \
      SLOT(updatePlayButton()), \
      SLOT(updateStopButton()), \
      SLOT(updateTraceWindowButton()), \
    }; \
    BOOST_FOREACH (const char *signal, status_signals) \
      BOOST_FOREACH (const char *slot, update_slots) \
        _connect(hub_, signal, this, slot); \
  }

  CREATE_HUB_CONNECTIONS(connect)
  CREATE_HUB_CONNECTIONS(disconnect)
#undef CREATE_HUB_CONNECTIONS

#define CREATE_PLAYER_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Player() \
  { \
    _connect(player_, SIGNAL(mediaTitleChanged(QString)), this, SLOT(updateTitle())); \
\
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(updateTitle())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(updateTitle())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(clearProgressMessage())); \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(updatePositionSlider())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(updatePositionSlider())); \
    _connect(player_, SIGNAL(mediaChanged()), this, SLOT(updatePositionButton())); \
    _connect(player_, SIGNAL(mediaClosed()), this, SLOT(updatePositionButton())); \
\
    _connect(player_, SIGNAL(lengthChanged()), this, SLOT(updatePositionButton())); \
    _connect(player_, SIGNAL(timeChanged()), this, SLOT(updatePositionButton())); \
    _connect(player_, SIGNAL(positionChanged()), this, SLOT(updatePositionSlider())); \
\
    _connect(player_, SIGNAL(playing()), this, SLOT(updateVolumeSlider())); \
 \
    static const char *status_signals[] = { \
      SIGNAL(playing()), SIGNAL(paused()), SIGNAL(stopped()) \
    }; \
    static const char *update_slots[] = { \
      SLOT(updatePlayButton()), \
      SLOT(updateStopButton()), \
      SLOT(updateNextFrameButton()), \
      SLOT(updateFastForwardButton()), \
      SLOT(updateFastFastForwardButton()), \
    }; \
    BOOST_FOREACH (const char *signal, status_signals) \
      BOOST_FOREACH (const char *slot, update_slots) \
        _connect(player_, signal, this, slot); \
  }

  CREATE_PLAYER_CONNECTIONS(connect)
  CREATE_PLAYER_CONNECTIONS(disconnect)
#undef CREATE_PLAYER_CONNECTIONS

#define CREATE_SERVER_CONNECTIONS(_connect) \
  void \
  PlayerUi::_connect##Server() \
  { \
    _connect(server_, SIGNAL(userChanged()), this, SLOT(updateUserButton())); \
  }

  CREATE_SERVER_CONNECTIONS(connect)
  CREATE_SERVER_CONNECTIONS(disconnect)
#undef CREATE_SERVER_CONNECTIONS

bool
PlayerUi::isValid() const
{ return hub_ && player_ && server_; }

void
PlayerUi::setTitle(const QString &title)
{ Base::setWindowTitle(title); }

void
PlayerUi::setVisible(bool visible)
{
  setActive(visible);
  //if (visible && !isVisible()) {
  //  static qreal opacity = 0.0;
  //  if (qFuzzyCompare(opacity + 1, 1))
  //    opacity = windowOpacity();
  //  if (!fadeAni_)
  //    fadeAni_ = new FadeAnimation(this, "windowOpacity", this);
  //  fadeAni_->fadeIn(opacity);
  //}
  Base::setVisible(visible);
}

void
PlayerUi::setActive(bool active)
{
  if (active) {
    updateTitle();
    updateVolumeSlider();
    updatePositionSlider();
    updatePositionButton();
    updateProgressButton();
    updateTraceWindowButton();
    updatePlayButton();
    updateStopButton();
    updateNextFrameButton();
    updateFastForwardButton();
    updateFastFastForwardButton();
    updateNextButton();
    updatePreviousButton();
    updatePlayerModeToggler();
    updateWindowModeToggler();
    updateInputCountButton();

    updateUserButton();

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
  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying())
      hub_->pause();
    else
      hub_->play();
    break;

  case SignalHub::MediaTokenMode:
    if (!player_->hasMedia())
      hub_->open();
    else if (player_->isValid())
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

  case SignalHub::LiveTokenMode:
    hub_->stop();
    hub_->setMediaTokenMode();
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
  if (hub_->isMediaTokenMode() &&
      player_->hasMedia())
    player_->nextFrame();
}

void
PlayerUi::fastForward(qreal rate)
{
  if (!player_->isStopped()) {
    player_->setRate(rate);
    if (player_->isPaused())
      player_->play();
  }
}

void
PlayerUi::fastForward()
{ fastForward(4.0); }
void
PlayerUi::fastFastForward()
{ fastForward(16.0); }

void
PlayerUi::rewind()
{
  fastForward(-1.0); // not supported by most codecs
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
{ hub_->setVolume(qreal(vol) / G_VOLUME_MAX); }

void
PlayerUi::setPosition(int pos)
{
  if (player_->hasMedia())
    player_->setPosition(qreal(pos) / G_POSITION_MAX);
}

void
PlayerUi::setProgressMessage(const QString &text)
{
  progressButton()->setText(text);
  bool v = hub_->isMediaTokenMode() &&
           hub_->isFullScreenWindowMode() &&
           !text.isEmpty();
  progressButton()->setVisible(v);
}

void
PlayerUi::updateVolumeSlider()
{
  QSlider *slider = volumeSlider();

  if (!hub_->isMediaTokenMode() || !player_->isValid()) {
    slider->setSliderPosition(0);
    if (slider->isEnabled())
      slider->setEnabled(false);
//#ifdef Q_OS_WIN
//    QtWin::repaintWindow(slider->winId());
//#endif // Q_OS_WIN
    return;
  }

  if (!slider->isEnabled())
    slider->setEnabled(true);

  qreal vol = hub_->volume();
  slider->setSliderPosition(vol * G_VOLUME_MAX);

  // Update tool tip.
  int percentage = qRound(vol * 100);
  slider->setToolTip(
    QString("%1 %2%" " [↑/↓]")
      .arg(TR(T_VOLUME))
      .arg(QString::number(percentage))
  );

//#ifdef Q_OS_WIN
//  QtWin::repaintWindow(slider->winId());
//#endif // Q_OS_WIN
}

void
PlayerUi::updatePositionSlider()
{
  //if (!active_)
  //  return;
  PositionSlider *slider = positionSlider();

  if (!hub_->isMediaTokenMode() || !player_->isValid()) {
    //slider->hide();
    slider->setSliderPosition(0);
    slider->clearAvailablePosition();
    if (slider->isEnabled())
      slider->setEnabled(false);
//#ifdef Q_OS_WIN
//    QtWin::repaintWindow(slider->winId());
//#endif // Q_OS_WIN
    return;
  }

  //slider->show();

  if (player_->hasMedia()) {
    if (!slider->isEnabled())
      slider->setEnabled(true);

    if (!slider->isSliderDown()) {
      qreal pos = player_->position();
      slider->setSliderPosition(pos * G_POSITION_MAX);
    }

    qreal progress = 0;
    if (player_->isDownloadFinished())
      slider->clearAvailablePosition();
    else {
      progress = player_->availablePosition();
      slider->setAvailablePosition(progress * G_POSITION_MAX);
    }

    // Update slider's tool tip and label's text.
    qint64 current_msecs = player_->time(),
           total_msecs = player_->mediaLength();
    qint64 left_msecs = total_msecs - current_msecs;

    QTime current = QtExt::msecs2time(current_msecs),
          left = QtExt::msecs2time(left_msecs);

    QString tip = current.toString() + " / -" + left.toString();
    if (total_msecs) {
      tip += QString(" (%1%").arg(QString::number(current_msecs * 100.0 / total_msecs, 'f', 1));
      if (!qFuzzyCompare(progress + 1, 1))
        tip += QString(" / %1%").arg(QString::number(progress * 100, 'f', 2));
      tip += ")";
    }
    slider->setToolTip(tip);

  } else {
    if (slider->isEnabled())
      slider->setEnabled(false);

    QString zero = QTime(0, 0, 0).toString();
    slider->setToolTip(zero + " / -" + zero);
  }
#ifdef Q_OS_WIN
  //QtWin::repaintWindow(slider->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateUserButton()
{
  QToolButton *b = userButton();
  if (server_->isAuthorized()) {
    b->setStyleSheet(ACSS_TOOLBUTTON_TEXT_NORMAL);
    b->setText(server_->user().name());
  } else {
    b->setStyleSheet(ACSS_TOOLBUTTON_TEXT_INVERT);
    b->setText(TR(T_LOGIN));
  }
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updatePositionButton()
{
  QToolButton *b = positionButton();

  if (player_->hasMedia()) {
    if (!b->isEnabled())
      b->setEnabled(true);

    // Update slider's tool tip and button's text.
    qint64 current_msecs = player_->time(),
           total_msecs = player_->mediaLength();
    //qint64 left_msecs = total_msecs - current_msecs;
    //QTime left = QtExt::msecs2time(left_msecs),

    QString current = QtExt::msecs2time(current_msecs).toString();
    QString t = current;
    if (total_msecs) {
      QString total = QtExt::msecs2time(total_msecs).toString();
      t.append(" / ").append(total);
    }

    b->setText(t);
  } else {
    if (b->isEnabled())
      b->setEnabled(false);

    QString zero = "--:--:--"; //QTime(0, 0, 0).toString();
    QString t = zero + " / " + zero;
    b->setText(t);
  }
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateProgressButton()
{
  progressButton()->setVisible(
    hub_->isMediaTokenMode() &&
    hub_->isFullScreenWindowMode()
  );
}

void
PlayerUi::updateTitle()
{
  if (!hub_->isMediaTokenMode())
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
PlayerUi::updateTraceWindowButton()
{
#ifdef WITH_WIN_PICKER
#endif // WITH_WIN_PICKER
}

void
PlayerUi::updatePlayButton()
{
  QToolButton *b = playButton();
  switch (hub_->tokenMode()) {
  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    if (hub_->isPlaying()) {
      b->setStyleSheet(SS_TOOLBUTTON_PAUSE);
      b->setToolTip(TR(T_TOOLTIP_PAUSE));
    } else {
      b->setStyleSheet(SS_TOOLBUTTON_PLAY);
      b->setToolTip(TR(T_TOOLTIP_PLAY));
    }
    break;

  case SignalHub::MediaTokenMode:
    switch (player_->status()) {
    case Player::Playing:
      b->setStyleSheet(SS_TOOLBUTTON_PAUSE);
      b->setToolTip(TR(T_TOOLTIP_PAUSE));
      break;
    case Player::Stopped:
    case Player::Paused:
      b->setStyleSheet(SS_TOOLBUTTON_PLAY);
      b->setToolTip(TR(T_TOOLTIP_PLAY));
      break;
    }
    break;
  }

#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateStopButton()
{
  QToolButton *b = stopButton();
  switch (hub_->tokenMode()) {
  case SignalHub::LiveTokenMode:
  case SignalHub::SignalTokenMode:
    b->setEnabled(!hub_->isStopped());
    break;

  case SignalHub::MediaTokenMode:
    b->setEnabled(!player_->isStopped());
    break;
  }
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateNextFrameButton()
{
  QToolButton *b = nextFrameButton();
  if (!hub_->isMediaTokenMode()) {
    b->hide();
    b->setEnabled(false);
    return;
  }

  switch (player_->status()) {
  case Player::Playing:
  case Player::Paused:
    b->setEnabled(true);
    break;
  case Player::Stopped:
    b->setEnabled(false);
    break;
  }

  b->show();
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateFastForwardButton()
{
  QToolButton *b = fastForwardButton();
  if (!hub_->isMediaTokenMode()) {
    b->hide();
    b->setEnabled(false);
    return;
  }

  switch (player_->status()) {
  case Player::Playing:
  case Player::Paused:
    b->setEnabled(true);
    break;
  case Player::Stopped:
    b->setEnabled(false);
    break;
  }

  b->show();
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}
void
PlayerUi::updateFastFastForwardButton()
{
  QToolButton *b = fastFastForwardButton();
  if (!hub_->isMediaTokenMode()) {
    b->hide();
    b->setEnabled(false);
    return;
  }

  switch (player_->status()) {
  case Player::Playing:
  case Player::Paused:
    b->setEnabled(true);
    break;
  case Player::Stopped:
    b->setEnabled(false);
    break;
  }

  b->show();
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updateNextButton()
{
  QToolButton *b = nextButton();
  if (!hub_->isMediaTokenMode()) {
    b->hide();
    b->setEnabled(false);
    return;
  }

  b->setEnabled(player_->hasMedia());
  b->show();
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

void
PlayerUi::updatePreviousButton()
{
  QToolButton *b = previousButton();
  if (!hub_->isMediaTokenMode()) {
    b->hide();
    b->setEnabled(false);
    return;
  }

  b->setEnabled(player_->hasMedia());
  b->show();
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}

// - Annotations -

void
PlayerUi::postAnnotation()
{
  QString text = inputComboBox()->currentText();
  if (text.isEmpty())
    return;

  inputComboBox()->clearEditText();

  QString prefix = prefixComboBox()->currentText();
  if (!prefix.trimmed().isEmpty()) {
    text = prefix + " " + text;

    QKeyEvent e(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(prefixComboBox()->lineEdit(), &e);
  }

  // TODO: use command filter rather than annnotation filter here
  if (Annotation::isValidText(text))
    emit textEntered(text);
}

// - Requests -

void
PlayerUi::clickUserButton()
{
  if (!server_->isAuthorized())
    emit loginRequested();
  else {
    emit updateUserMenuRequested();
    userButton()->showMenu();
  }
}

void
PlayerUi::updatePlayerModeToggler()
{
  toggleMiniModeButton()->setChecked(hub_->isMiniPlayerMode());
  toggleEmbedModeButton()->setChecked(hub_->isEmbeddedPlayerMode());
}

void
PlayerUi::updateWindowModeToggler()
{ toggleFullScreenModeButton()->setChecked(hub_->isFullScreenWindowMode()); }

void
PlayerUi::updateVisibleWidgets()
{
  bool v = hub_->isMediaTokenMode();
  volumeSlider()->setVisible(v);
  positionSlider()->setVisible(v);
  positionButton()->setVisible(v);

  previousButton()->setVisible(v);
  nextButton()->setVisible(v);
  nextFrameButton()->setVisible(v);
}

void
PlayerUi::updateInputCountButton()
{
  QToolButton *b = inputCountButton();
  enum { total = 255 };
  int count = currentText().size();
  QString text = QString("%1/%2").arg(count).arg(total);
  b->setText(text);

  b->setStyleSheet(count < total ?
      ACSS_TOOLBUTTON_TEXT :
      ACSS_TOOLBUTTON_TEXT_INVERT);
}

// - Menu button -

void
PlayerUi::popupMenu()
{
  emit updateMenuRequested();
  menuButton()->showMenu();
}

void
PlayerUi::popupInputItems()
{ inputComboBox()->showPopup(); }

QString
PlayerUi::currentText() const
{
  Self *self = const_cast<Self *>(this);
  QString x = self->prefixComboBox()->currentText().trimmed(),
          y = self->inputComboBox()->currentText().trimmed();
  if (x.isEmpty())
    return y;
  else
    return x + " " + y;
}

// EOF

/*
void
PlayerUi::setAnnotationEnabled(bool enabled)
{
  QToolButton *b = toggleAnnotationButton();
  if (enabled) {
    b->setStyleSheet(SS_TOOLBUTTON_HIDEANNOT);
    b->setToolTip(TR(T_TOOLTIP_HIDEANNOT));
  } else {
    b->setStyleSheet(SS_TOOLBUTTON_SHOWANNOT);
    b->setToolTip(TR(T_TOOLTIP_SHOWANNOT));
  }
#ifdef Q_OS_WIN
  QtWin::repaintWindow(b->winId());
#endif // Q_OS_WIN
}
*/
