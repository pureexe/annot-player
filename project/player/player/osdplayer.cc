// osdplayer.cc
// 7/16/2011

#include "osdplayer.h"
#include "global.h"
#include "stylesheet.h"
#include <QtGui>

// - Constructions -
OSDPlayerUi::OSDPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent)
{
  setWindowFlags(Qt::FramelessWindowHint);
  setContentsMargins(0, 0, 0, 0);
  createLayout();

  // Auto hide player.
  autoHideTimer_ = new QTimer(this);
  autoHideTimer_->setInterval(G_AUTOHIDE_TIMEOUT);
  connect(autoHideTimer_, SIGNAL(timeout()), SLOT(autoHide()));

  connect(prefixLineEdit(), SIGNAL(textChanged(QString)), SLOT(resetAutoHideTimeoutWhenEditing(QString)));
  connect(lineEdit(), SIGNAL(textChanged(QString)), SLOT(resetAutoHideTimeoutWhenEditing(QString)));
  connect(prefixLineEdit(), SIGNAL(cursorPositionChanged(int,int)), SLOT(resetAutoHideTimeoutWhenEditing(int,int)));
  connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)), SLOT(resetAutoHideTimeoutWhenEditing(int,int)));
}

void
OSDPlayerUi::createLayout()
{
  // Reset Ui style
  prefixLineEdit()->setStyleSheet(SS_PREFIXLINEEDIT_OSD);
  lineEdit()->setStyleSheet(SS_LINEEDIT_OSD);

  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row = new QHBoxLayout;
    rows->addWidget(positionSlider());
    rows->addLayout(row);

    row->addWidget(playButton());
    row->addWidget(toggleAnnotationButton());
    row->addWidget(nextFrameButton());
    row->addWidget(stopButton());
    row->addWidget(openButton());
    row->addWidget(togglePlayModeButton());
    row->addWidget(toggleMiniModeButton());
    row->addStretch();
    row->addWidget(userButton());
    row->addWidget(prefixLineEdit());
    row->addWidget(lineEdit());
    row->addWidget(volumeSlider());
    row->addWidget(positionButton());
  }
  setLayout(rows);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

// - Auto hide -

void
OSDPlayerUi::autoHide()
{
  if (underMouse() || lineEdit()->hasFocus() || prefixLineEdit()->hasFocus())
    resetAutoHideTimeout();
  else if (isVisible())
    hide();
}

bool
OSDPlayerUi::autoHideEnabled() const
{
  Q_ASSERT(autoHideTimer_);
  return autoHideTimer_->isActive();
}

void
OSDPlayerUi::setAutoHideEnabled(bool enabled)
{
  Q_ASSERT(autoHideTimer_);
  if (enabled) {
    if (!autoHideTimer_->isActive())
      autoHideTimer_->start();
  } else  {
    if (autoHideTimer_->isActive())
      autoHideTimer_->stop();
  }
}

void
OSDPlayerUi::resetAutoHideTimeout()
{
  Q_ASSERT(autoHideTimer_);
  if (autoHideTimer_->isActive())
    autoHideTimer_->stop();
  autoHideTimer_->start();
}

// - Geometry -

void
OSDPlayerUi::invalidateGeometry()
{
  QWidget *widget = qobject_cast<QWidget*>(parent());
  if (!widget)
    return;

  // Invalidate size
  int w_max = widget->width();
  int h_hint = sizeHint().height();
  resize(w_max, h_hint);

  // Invalidate position
  //int x_center = widget->width() / 2 - 2 * width();
  int x_left = 0;
  int y_bottom = widget->height() - height();
  move(x_left, y_bottom);
}

// EOF
