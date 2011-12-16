// miniplayer.cc
// 7/11/2011

#include "miniplayer.h"
#include "global.h"
#include "signalhub.h"
#include "uistyle.h"
#include "module/player/player.h"    // TO BE REMOVED; added due to bad design of playerui.
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_WS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( WINDOW_FLAGS_BASE )
#endif // Q_WS_MAC

// - Constructions -

MiniPlayerUi::MiniPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent), dragPos_(BAD_POS)
{
  setWindowFlags(WINDOW_FLAGS);
  setContentsMargins(0, 0, 0, 0);
  UiStyle::globalInstance()->setWindowStyle(this);
  setAcceptDrops(true);

  createLayout();
}

void
MiniPlayerUi::createLayout()
{
  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row0 = new QHBoxLayout,
                *row1 = new QHBoxLayout,
                *row2 = new QHBoxLayout;
    rows->addLayout(row0);
    rows->addLayout(row1);
    rows->addLayout(row2);

    row0->addWidget(positionButton());
    row0->addWidget(positionSlider());

    row1->addWidget(playButton());
    row1->addWidget(toggleAnnotationButton());
    row1->addWidget(nextFrameButton());
    row1->addWidget(stopButton());
    row1->addWidget(openButton());
    row1->addWidget(previousButton());
    row1->addWidget(nextButton());
    row1->addWidget(toggleEmbedModeButton());
    row1->addWidget(toggleMiniModeButton());
    row1->addWidget(toggleFullScreenModeButton());
    row1->addWidget(volumeSlider());

    row2->addWidget(userButton());
    row2->addWidget(prefixComboBox());
    row2->addWidget(inputComboBox());

    // void setContentsMargins(int left, int top, int right, int bottom);
    rows->setContentsMargins(9, 9, 9, 0);
    row0->setContentsMargins(0, 0, 0, 0);
    row1->setContentsMargins(0, 0, 0, 9);
    row2->setContentsMargins(0, 0, 0, 9);
  }
  setLayout(rows);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  // TOD jichi 7/26/2011: This is really a bad hotfix. Need a better design for PlayerUI class do to this in an efficient way.
  // Note: there is no textChanged event in QLabel.
  positionButton()->hide(); // always hide - it is better to disable this label rather than hide it.
  positionButton()->resize(0, 0);
}

void
MiniPlayerUi::setVisible(bool visible)
{
  if (visible) {
    if (!isActive()) {
      connect(player(), SIGNAL(lengthChanged()), SLOT(invalidateTitle()));
      connect(player(), SIGNAL(timeChanged()), SLOT(invalidateTitle()));
    }
  } else {
    if (isActive()) {
      disconnect(player(), SIGNAL(lengthChanged()), this, SLOT(invalidateTitle()));
      disconnect(player(), SIGNAL(timeChanged()), this, SLOT(invalidateTitle()));
    }
  }

  Base::setVisible(visible);
}

// BAD DESIGN! TO BE REMOVED!
void
MiniPlayerUi::invalidateTitle()
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
    QApplication::sendEvent(parent(), event);
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
      hub()->toggleMiniPlayerMode();
      event->accept();
      break;

    default: break;
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
  //MiniPlayerUi *ui = qobject_cast<MiniPlayerUi*>(widget());
  //if (ui)
  //  ui->setActive(visible);
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

    default: break;
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
    QApplication::sendEvent(parent(), event);
}

void MiniPlayerDock::contextMenuEvent(QContextMenuEvent *event) { forward(event); }
void MiniPlayerDock::dragEnterEvent(QDragEnterEvent *event)     { forward(event); }
void MiniPlayerDock::dragMoveEvent(QDragMoveEvent *event)       { forward(event); }
void MiniPlayerDock::dragLeaveEvent(QDragLeaveEvent *event)     { forward(event); }
void MiniPlayerDock::dropEvent(QDropEvent *event)               { forward(event); }
void MiniPlayerDock::keyPressEvent(QKeyEvent *event)            { forward(event); }
void MiniPlayerDock::keyReleaseEvent(QKeyEvent *event)          { forward(event); }
*/

