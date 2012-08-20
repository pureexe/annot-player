// miniplayer.cc
// 7/11/2011

#include "miniplayer.h"
#include "global.h"
#include "signalhub.h"
#include "positionslider.h"
#include "project/common/acui.h"
#include "module/player/player.h"
#include "module/qtext/overlaylayout.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include <QtCore>
#include <QtGui>

#define WINDOW_OPACITY  0.7

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
# define WINDOW_FLAGS WINDOW_FLAGS_BASE
#endif // Q_OS_MAC

enum { INPUTLINE_MAXIMUM_WIDTH = 300 };
enum { PLAY_BUTTON_SIZE = 30 };

// - Constructions -

MiniPlayerUi::MiniPlayerUi(SignalHub *hub, Player *player, AnnotationServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent), dragPos_(BAD_POS)
{
  Qt::WindowFlags f = WINDOW_FLAGS;
#ifdef Q_OS_WIN
  if (QtWin::isWindowsVistaOrLater()) {
    f |= Qt::WindowTitleHint;
    setWindowOpacity(AC_WINDOW_OPACITY);
  } else
#endif // Q_OS_WIN
  { setWindowOpacity(WINDOW_OPACITY); }
  setWindowFlags(f);
  setContentsMargins(0, 0, 0, 0);
  //setAcceptDrops(true);
  AcUi::globalInstance()->setWindowStyle(this);

  createLayout();
  setTabOrder(inputComboBox(), prefixComboBox());

  connect(new QShortcut(QKeySequence("ESC"), this), SIGNAL(activated()), hub, SLOT(toggleEmbeddedPlayerMode()));
  connect(new QShortcut(QKeySequence("CTRL+1"), this), SIGNAL(activated()), hub, SLOT(toggleEmbeddedPlayerMode()));
  connect(new QShortcut(QKeySequence("CTRL+2"), this), SIGNAL(activated()), hub, SLOT(toggleMiniPlayerMode()));
  connect(new QShortcut(QKeySequence("CTRL+3"), this), SIGNAL(activated()), hub, SLOT(toggleFullScreenWindowMode()));
#ifndef Q_OS_MAC
  connect(new QShortcut(QKeySequence("ALT+1"), this), SIGNAL(activated()), hub, SLOT(toggleEmbeddedPlayerMode()));
  connect(new QShortcut(QKeySequence("ALT+2"), this), SIGNAL(activated()), hub, SLOT(toggleMiniPlayerMode()));
  connect(new QShortcut(QKeySequence("ALT+3"), this), SIGNAL(activated()), hub, SLOT(toggleFullScreenWindowMode()));
#endif // Q_OS_MAC
}

void
MiniPlayerUi::createLayout()
{
  inputComboBox()->setMaximumWidth(INPUTLINE_MAXIMUM_WIDTH);

  playButton()->setProperty("radius", int(PLAY_BUTTON_SIZE));

  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row0 = new QHBoxLayout,
                *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout;
    rows->addLayout(row0);
    rows->addLayout(row1);
    rows->addLayout(row2);

    row0->addWidget(positionSlider());

    //row1->addWidget(menuButton());
    row1->addWidget(openButton());
    row1->addWidget(playButton());
    //row1->addWidget(toggleAnnotationButton());
    row1->addWidget(nextFrameButton());
    row1->addWidget(fastForwardButton());
    //row1->addWidget(fastFastForwardButton());
    row1->addWidget(stopButton());
    row1->addWidget(previousButton());
    row1->addWidget(nextButton());
    row1->addWidget(toggleEmbedModeButton());
    row1->addWidget(toggleMiniModeButton());
    row1->addWidget(toggleFullScreenModeButton());
    row1->addWidget(toggleTraceWindowButton());
    row1->addWidget(positionButton());
    row1->addWidget(volumeSlider());
    row1->addWidget(libraryButton());

    row2->addWidget(userButton());
    row2->addWidget(prefixComboBox());

    OverlayLayout *input = new OverlayLayout;
    input->addWidget(inputComboBox());
    input->addWidget(inputCountButton(), Qt::AlignRight);
    row2->addLayout(input);

    row2->addWidget(networkButton());

    // void setContentsMargins(int left, int top, int right, int bottom);
    rows->setContentsMargins(9, 9, 9, 0);
    row0->setContentsMargins(0, 0, 0, 0);
    row1->setContentsMargins(0, 0, 0, 1);
    row2->setContentsMargins(0, 1, 0, 9);
  }
  setLayout(rows);
  //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // TOD jichi 7/26/2011: This is really a bad hotfix. Need a better design for PlayerUI class do to this in an efficient way.
  // Note: there is no textChanged event in QLabel.
  progressButton()->hide();
  progressButton()->resize(QSize());
  //fastFastForwardButton()->hide();
  //fastFastForwardButton()->resize(QSize());
  menuButton()->hide();
  menuButton()->resize(QSize());
#ifndef WITH_WIN_PICKER
  toggleTraceWindowButton()->hide();
  toggleTraceWindowButton()->resize(QSize());
#endif // WITH_WIN_PICKER
#ifdef Q_OS_WIN
  if (QtWin::isWindowsVistaOrLater()) {
    positionButton()->hide();
    positionButton()->resize(QSize());
  } else
#endif // Q_OS_WIN
  {
    volumeSlider()->hide();
    volumeSlider()->resize(QSize());
  }

  //setStyleSheet(styleSheet() +
  //  SS_BEGIN(QToolButton)
  //    SS_NO_BORDER
  //  SS_END
  //);
}

void
MiniPlayerUi::setVisible(bool visible)
{
  if (visible) {
    if (!isActive()) {
      connect(player(), SIGNAL(lengthChanged()), SLOT(updateTitle()));
      connect(player(), SIGNAL(timeChanged()), SLOT(updateTitle()));
    }
  } else {
    if (isActive()) {
      disconnect(player(), SIGNAL(lengthChanged()), this, SLOT(updateTitle()));
      disconnect(player(), SIGNAL(timeChanged()), this, SLOT(updateTitle()));
    }
  }
  Base::setVisible(visible);
}

// BAD DESIGN! TO BE REMOVED!
void
MiniPlayerUi::updateTitle()
{
  if (hub()->isMediaTokenMode())
    setWindowTitle(positionButton()->text());
}

// - Properties -

void
MiniPlayerUi::setTitle(const QString &title)
{ Base::setWindowTitle(title); }

// - Events -

void
MiniPlayerUi::forward(QEvent *event)
{
  // Forward event to parent
  if (event && parent())
    QCoreApplication::sendEvent(parent(), event);
}

void MiniPlayerUi::contextMenuEvent(QContextMenuEvent *event) { forward(event); }
void MiniPlayerUi::keyPressEvent(QKeyEvent *event)            { forward(event); }
void MiniPlayerUi::keyReleaseEvent(QKeyEvent *event)          { forward(event); }

void MiniPlayerUi::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void MiniPlayerUi::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void MiniPlayerUi::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void MiniPlayerUi::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

void
MiniPlayerUi::mousePressEvent(QMouseEvent *event)
{
  if (event)
    switch (event->button()) {
    case Qt::LeftButton:
      if (dragPos_ == BAD_POS)
        dragPos_ = event->globalPos() - frameGeometry().topLeft();
      event->accept();
      break;

    case Qt::MiddleButton:
      if (hub()->isSignalTokenMode() || player()->isPlaying())
        hub()->setEmbeddedPlayerMode();
      else
        hub()->setNormalPlayerMode();
      event->accept();
      break;

    default: ;
    }
}

void
MiniPlayerUi::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
MiniPlayerUi::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = BAD_POS;
  if (event)
    event->accept();
}

void
MiniPlayerUi::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event && event->buttons() == Qt::LeftButton) {
    hub()->toggleFullScreenWindowMode();
    event->accept();
  }
}

// EOF

// - MiniPlayerDock -

/*
// ++ Constructions ++
MiniPlayerDock::MiniPlayerDock(QWidget *parent)
  : Base(parent), dragPos_(BAD_POS)
{
  //setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
  setFeatures(QDockWidget::DockWidgetFloatable);
  setFloating(true);

  UiStyle::globalInstance()->setWindowStyle(this);

  //setTitleBarWidget(new QWidget(this));
  //setWindowFlags(Qt::FramelessWindowHint);
  setAllowedAreas(Qt::NoDockWidgetArea);
  setContentsMargins(0, 0, 0, 0);
}

void
MiniPlayerDock::setVisible(bool visible)
{
  //auto p = qobject_cast<MiniPlayerUi *>(widget());
  //if (p)
  //  p->setActive(visible);
  if (widget())
    widget()->setVisible(visible);
  Base::setVisible(visible);
  //if (visible) {
  //  setWindowFlags(Qt::WindowMinimizeButtonHint);
  //}
}

// ++ Events ++
void
MiniPlayerDock::mousePressEvent(QMouseEvent *event)
{
  if (event)
    switch (event->button()) {
    case Qt::LeftButton:
      if (dragPos_ == BAD_POS)
        dragPos_ = event->globalPos() - frameGeometry().topLeft();
      event->accept();
      break;

    case Qt::MiddleButton:
      emit toggleMiniModeRequested();
      event->accept();
      break;

    default: ;
    }
}

void
MiniPlayerDock::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton
      && dragPos_ != BAD_POS) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
MiniPlayerDock::mouseReleaseEvent(QMouseEvent *event)
{
  dragPos_ = BAD_POS;
  if (event)
    event->accept();
}

void
MiniPlayerDock::mouseDoubleClickEvent(QMouseEvent *event)
{
  if (event && event->buttons() == Qt::LeftButton) {
    emit toggleFullScreenModeRequested();
    event->accept();
  }
}

void
MiniPlayerDock::closeEvent(QCloseEvent *event)
{
  if (event && isVisible()) {
    emit toggleMiniModeRequested();
    event->accept();
  }
}

void
MiniPlayerDock::forward(QEvent *event)
{
  // Forward event to parent
  Q_ASSERT(event);
  if (parent())
    QCoreApplication::sendEvent(parent(), event);
}

void MiniPlayerDock::contextMenuEvent(QContextMenuEvent *event) { forward(event); }
void MiniPlayerDock::dragEnterEvent(QDragEnterEvent *event)     { forward(event); }
void MiniPlayerDock::dragMoveEvent(QDragMoveEvent *event)       { forward(event); }
void MiniPlayerDock::dragLeaveEvent(QDragLeaveEvent *event)     { forward(event); }
void MiniPlayerDock::dropEvent(QDropEvent *event)               { forward(event); }
void MiniPlayerDock::keyPressEvent(QKeyEvent *event)            { forward(event); }
void MiniPlayerDock::keyReleaseEvent(QKeyEvent *event)          { forward(event); }
*/

