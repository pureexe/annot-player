// embeddedplayer.cc
// 7/16/2011

#include "embeddedplayer.h"
#include "global.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "signalhub.h"
#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#include "core/gui/toolbutton.h"
#include <QtGui>

#define DEBUG "EmbeddedPlayerUi"
#include "module/debug/debug.h"

// - Constructions -
EmbeddedPlayerUi::EmbeddedPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent)
  : Base(hub, player, server, parent),
    menuButton_(0), containerWindow_(0), containerWidget_(0),
    fullScreen_(false), top_(false)
{
  setWindowFlags(Qt::FramelessWindowHint);
  setContentsMargins(0, 0, 0, 0);
  createLayout();

  // Auto hide player.
  autoHideTimer_ = new QTimer(this);
  autoHideTimer_->setInterval(G_AUTOHIDE_TIMEOUT);
  connect(autoHideTimer_, SIGNAL(timeout()), SLOT(autoHide()));

  trackingTimer_ = new QTimer(this);
  trackingTimer_->setInterval(G_TRACKING_INTERVAL);
  connect(trackingTimer_, SIGNAL(timeout()), SLOT(invalidateGeometry()));

  connect(prefixLineEdit(), SIGNAL(textChanged(QString)), SLOT(resetAutoHideTimeoutWhenEditing(QString)));
  connect(lineEdit(), SIGNAL(textChanged(QString)), SLOT(resetAutoHideTimeoutWhenEditing(QString)));
  connect(prefixLineEdit(), SIGNAL(cursorPositionChanged(int,int)), SLOT(resetAutoHideTimeoutWhenEditing(int,int)));
  connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)), SLOT(resetAutoHideTimeoutWhenEditing(int,int)));

  resize(0, 0); // temporarily
}

void
EmbeddedPlayerUi::createLayout()
{
  // Reset Ui style
  prefixLineEdit()->setStyleSheet(SS_PREFIXLINEEDIT_OSD);
  lineEdit()->setStyleSheet(SS_LINEEDIT_OSD);

  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row = new QHBoxLayout;
    rows->addWidget(positionSlider());
    rows->addLayout(row);

    row->addWidget(menuButton());
    row->addWidget(playButton());
    row->addWidget(toggleAnnotationButton());
    row->addWidget(nextFrameButton());
    row->addWidget(stopButton());
    row->addWidget(openButton());
    row->addWidget(previousButton());
    row->addWidget(nextButton());
    row->addWidget(toggleEmbedModeButton());
    row->addWidget(toggleMiniModeButton());
    row->addWidget(toggleFullScreenModeButton());
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

// - Properties -

bool
EmbeddedPlayerUi::isFullScreenMode() const
{ return fullScreen_; }

void
EmbeddedPlayerUi::setFullScreenMode(bool t)
{
  if (fullScreen_ != t) {
    fullScreen_ = t;
    invalidateTrackingTimer();
    invalidateGeometry();
    emit fullScreenModeChanged(fullScreen_);
  }
}

bool
EmbeddedPlayerUi::isOnTop() const
{ return top_; }

void
EmbeddedPlayerUi::setOnTop(bool t)
{
  if (top_ != t) {
    top_ = t;
    invalidateGeometry();
  }
}

// - Auto hide -

void
EmbeddedPlayerUi::autoHide()
{
  if (underMouse() || lineEdit()->hasFocus() || prefixLineEdit()->hasFocus())
    resetAutoHideTimeout();
  else if (isVisible())
    hide();
}

bool
EmbeddedPlayerUi::autoHideEnabled() const
{
  Q_ASSERT(autoHideTimer_);
  return autoHideTimer_->isActive();
}

void
EmbeddedPlayerUi::setAutoHideEnabled(bool enabled)
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
EmbeddedPlayerUi::resetAutoHideTimeout()
{
  Q_ASSERT(autoHideTimer_);
  if (autoHideTimer_->isActive())
    autoHideTimer_->stop();
  autoHideTimer_->start();
}

// - Geometry -

void
EmbeddedPlayerUi::invalidateGeometry()
{
  if (fullScreen_ && QApplication::desktop()) {
    QRect r = QApplication::desktop()->screenGeometry(this);
    Q_ASSERT(!r.isNull);

    // Invalidate size
    int w_max = r.width();
    int h_hint = sizeHint().height();
    QSize newSize(w_max, h_hint);
    if (newSize != size())
      resize(newSize);

    int x_left = r.x();
    int y = top_ ? r.y() :
                   r.y() + r.height() - height();
    moveToGlobalPos(QPoint(x_left, y));

  } else if (containerWindow_) {
#ifdef Q_WS_WIN
    QRect r = QtWin::getWindowRect(containerWindow_);
    if (r.isNull()) {
      if (!QtWin::isValidWindow(containerWindow_))
        setContainerWindow(0);

    } else {
      int w_max = r.width();
      int h_hint = sizeHint().height();
      QSize newSize(w_max, h_hint);
      if (newSize != size())
        resize(newSize);

      //int window_header_height = hub()->isSignalTokenMode() ? 40 : 0;
      enum { window_header_height = 40 };

      int x_left = r.x();
      int y = top_ ? r.top() + window_header_height : r.bottom() - height();

      moveToGlobalPos(QPoint(x_left, y));
    }
#endif // Q_WS_WIN
  } else {
    QWidget *w = containerWidget_ ? containerWidget_ : parentWidget();
    if(w) {
      // Invalidate size
      int w_max = w->width();
      int h_hint = sizeHint().height();
      QSize newSize(w_max, h_hint);
      if (newSize != size())
        resize(newSize);

      // Invalidate position
      //int x_center = widget->width() / 2 - 2 * width();
      QPoint g = w->mapToGlobal(QPoint());
      int x_left = g.x();
      int y = top_ ? g.y() :
                     g.y() + w->height() - height();
      moveToGlobalPos(QPoint(x_left, y));
    }
  }
}

void
EmbeddedPlayerUi::moveToGlobalPos(const QPoint &globalPos)
{
  // Currently only work on Windows
  QPoint newPos = frameGeometry().topLeft() + pos() // relative position
                  + globalPos - mapToGlobal(pos()); // absolute distance
  if (newPos != pos())
    move(newPos);
}

WId
EmbeddedPlayerUi::containerWindow() const
{ return containerWindow_; }

QWidget*
EmbeddedPlayerUi::containerWidget() const
{ return containerWidget_; }

void
EmbeddedPlayerUi::setContainerWindow(WId winId)
{
  if (containerWindow_ != winId) {
    containerWindow_ = winId;
    invalidateTrackingTimer();
  }
}

void
EmbeddedPlayerUi::setContainerWidget(QWidget *w)
{
  if (containerWidget_ != w) {
    containerWidget_ = w;
    invalidateTrackingTimer();
  }
}

void
EmbeddedPlayerUi::invalidateTrackingTimer()
{
  if (isVisible() && !fullScreen_ &&
      (containerWindow_ || containerWidget_))
    startTracking();
  else
    stopTracking();
}


void
EmbeddedPlayerUi::startTracking()
{
  if (!trackingTimer_->isActive())
    trackingTimer_->start();
}

void
EmbeddedPlayerUi::stopTracking()
{
  if (trackingTimer_->isActive())
    trackingTimer_->stop();
}

void
EmbeddedPlayerUi::setVisible(bool visible)
{
  if (visible == isVisible())
    return;

  if (visible)
    invalidateGeometry();

  Base::setVisible(visible);

  invalidateTrackingTimer();
}

// - Menu button -

void
EmbeddedPlayerUi::setMenu(QMenu *menu)
{ menuButton()->setMenu(menu); }

QToolButton*
EmbeddedPlayerUi::menuButton()
{
  if (!menuButton_) {
    menuButton_ = new Core::Gui::ToolButton(this);
    menuButton_->setStyleSheet(SS_TOOLBUTTON_MENU);
    menuButton_->setToolTip(TR(T_TOOLTIP_MENU));
    UiStyle::globalInstance()->setToolButtonStyle(menuButton_);

    connect(menuButton_, SIGNAL(clicked()), SLOT(popupMenu()));
  }
  return menuButton_;
}

void
EmbeddedPlayerUi::popupMenu()
{
  emit invalidateMenuRequested();
  menuButton()->showMenu();
}

// EOF
