// embeddedplayer.cc
// 7/16/2011

#include "embeddedplayer.h"
#include "embeddedcanvas.h"
#include "embeddedinfoview.h"
#include "positioncalibration.h"
#include "positionslider.h"
#include "global.h"
#include "tr.h"
#include "stylesheet.h"
#include "signalhub.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#include "project/common/acui.h"
#include "project/common/aciconbutton.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/withsizehint.h"
#include "module/qtext/overlaylayout.h"
#include <QtCore>
#include <QtGui>

#define DEBUG "embeddedplayerui"
#include "module/debug/debug.h"

enum { INPUTLINE_MINWIDTH = 400, INPUTLINE_MINHEIGHT = 25 };
enum { VolumeSliderMaximumWidth = 100 };

// Style sheet of entire embedded player
#define SS_PLAYER \
  SS_BEGIN(QWidget) \
    SS_SEMI_TRANSPARENT \
  SS_END \
  SS_BEGIN(QToolButton) \
    SS_NO_BORDER \
  SS_END

#define SS_LINEEDIT_OSD \
  SS_BEGIN(QLineEdit) \
    SS_SEMI_TRANSPARENT \
    SS_BORDER(1px groove purple) \
    SS_COLOR(blue) \
  SS_END \
  SS_BEGIN(QLineEdit::hover) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_LINEEDIT) \
    SS_BORDER(1px groove black) \
    SS_COLOR(black) \
  SS_END

#define SS_COMBOBOX_OSD \
  SS_BEGIN(QComboBox) \
    SS_SEMI_TRANSPARENT \
    SS_COLOR(blue) \
  SS_END \
  SS_BEGIN(QComboBox QAbstractItemView) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
  SS_END

#define SS_COMBOBOX_OSD_NODROPDOWN \
  SS_BEGIN(QComboBox) \
    SS_SEMI_TRANSPARENT \
    SS_COLOR(blue) \
  SS_END \
  SS_BEGIN(QComboBox QAbstractItemView) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
  SS_END \
  SS_BEGIN(QComboBox::drop-down) \
    SS_TRANSPARENT \
  SS_END \
  SS_BEGIN(QComboBox::down-arrow) \
    SS_TRANSPARENT \
  SS_END \
  SS_BEGIN(QComboBox::down-arrow:pressed) \
    SS_TRANSPARENT \
  SS_END

#define SS_TOOLBUTTON_DOCK_TOP      SS_TOOLBUTTON_UPARROW
#define SS_TOOLBUTTON_DOCK_BOTTOM   SS_TOOLBUTTON_DOWNARROW

// - Constructions -
EmbeddedPlayerUi::EmbeddedPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, DataManager *data, QWidget *parent)
  : Base(hub, player, server, parent),
    containerWindow_(0), containerWidget_(nullptr),
    fullScreen_(false), top_(false)
{
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  infoView_ = new EmbeddedInfoView(player, data, hub, this);

  canvas_ = new EmbeddedCanvas(data, hub, player);
  canvas_->hide();
  connect(canvas_, SIGNAL(visibleChanged(bool)), SLOT(hideInfoView(bool)));
  connect(canvas_, SIGNAL(visibleChanged(bool)), SLOT(updateGeometry()), Qt::QueuedConnection);

  calibration_ = new PositionCalibration(hub, player, this);

  toggleDockButton_ = new AcIconButton(this); {
    toggleDockButton_->setCheckable(true);
    toggleDockButton_->setChecked(true);
    toggleDockButton_->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(QColor("orange")));
    connect(toggleDockButton_, SIGNAL(clicked()), SLOT(toggleOnTop()));
    updateDockButton();
  }

  createLayout();

  setTabOrder(inputComboBox(), prefixComboBox());

  // Auto hide player.
  autoHideTimer_ = new QTimer(this);
  autoHideTimer_->setInterval(G_AUTOHIDE_TIMEOUT);
  connect(autoHideTimer_, SIGNAL(timeout()), SLOT(autoHide()));

  trackingTimer_ = new QTimer(this);
  trackingTimer_->setInterval(G_TRACKING_INTERVAL);
  connect(trackingTimer_, SIGNAL(timeout()), SLOT(updateGeometry()));

#define CONNECT_TO_AUTOHIDE(_obj, _signal) \
  connect(_obj, _signal, SLOT(resetAutoHideTimeout())); \
  connect(_obj, _signal, SLOT(showWhenEmbedded()));

  CONNECT_TO_AUTOHIDE(prefixComboBox()->lineEdit(), SIGNAL(cursorPositionChanged(int,int)));
  CONNECT_TO_AUTOHIDE(inputComboBox()->lineEdit(), SIGNAL(cursorPositionChanged(int,int)));
#undef CONNECT_TO_AUTOHIDE

  resize(QSize()); // temporarily

  connect(new QShortcut(QKeySequence("F2"), this), SIGNAL(activated()), hub, SLOT(toggleEmbeddedPlayerMode()));
  connect(new QShortcut(QKeySequence("F3"), this), SIGNAL(activated()), hub, SLOT(toggleMiniPlayerMode()));
  connect(new QShortcut(QKeySequence("F11"), this), SIGNAL(activated()), hub, SLOT(toggleFullScreenWindowMode()));
}

void
EmbeddedPlayerUi::createLayout()
{
  // Reset Ui style
  inputComboBox()->setStyleSheet(SS_COMBOBOX_OSD_NODROPDOWN);
  inputComboBox()->lineEdit()->setStyleSheet(SS_LINEEDIT_OSD);
  prefixComboBox()->setStyleSheet(SS_COMBOBOX_OSD);
  prefixComboBox()->lineEdit()->setStyleSheet(SS_LINEEDIT_OSD);

  {
    //enum { ButtonSize = 25 };
    //playButton()->setProperty("radius", int(ButtonSize));
    //playButton()->setProperty("radius", int(ButtonSize));
    //openButton()->setProperty("radius", int(ButtonSize));
    //playButton()->setProperty("radius", int(ButtonSize));
    //stopButton()->setProperty("radius", int(ButtonSize));
    //nextFrameButton()->setProperty("radius", int(ButtonSize));
    //fastForwardButton()->setProperty("radius", int(ButtonSize));
    //toggleFullScreenModeButton()->setProperty("radius", int(ButtonSize));
    //toggleMiniModeButton()->setProperty("radius", int(ButtonSize));
    //toggleEmbedModeButton()->setProperty("radius", int(ButtonSize));
    //positionButton()->setProperty("radius", int(ButtonSize));
    //progressButton()->setProperty("radius", int(ButtonSize));
    //previousButton()->setProperty("radius", int(ButtonSize));
    //nextButton()->setProperty("radius", int(ButtonSize));
    //menuButton()->setProperty("radius", int(ButtonSize));

    enum { PlayButtonSize = 35 };
    playButton()->setProperty("radius", int(PlayButtonSize));
    //playButton()->setProperty("minimumRadius", int(PlayButtonMinimumSize));
  }

  auto
  w = dynamic_cast<QtExt::WithSizeHint *>(inputComboBox());
  Q_ASSERT(w);
  w->setSizeHint(QSize(INPUTLINE_MINWIDTH, INPUTLINE_MINHEIGHT));
  w = dynamic_cast<QtExt::WithSizeHint *>(prefixComboBox());
  Q_ASSERT(w);
  w->setSizeHint(QSize(G_PREFIXLINE_MAXWIDTH, INPUTLINE_MINHEIGHT));

  volumeSlider()->setMaximumWidth(VolumeSliderMaximumWidth);

  //AcTextView *info_ = new AcTextView(this);
  //info_->setText("awfwaefawfew\n22222", Qt::red);

  // Set layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row = new QHBoxLayout;
    rows->addWidget(infoView_);
    rows->addWidget(canvas_);
    rows->addWidget(calibration_);
    rows->addWidget(positionSlider());
    rows->addLayout(row);

    row->addWidget(toggleDockButton_);
    row->addWidget(menuButton());
    row->addWidget(openButton());
    row->addWidget(playButton());
    //row->addWidget(toggleAnnotationButton());
    row->addWidget(nextFrameButton());
    row->addWidget(fastForwardButton());
    row->addWidget(stopButton());
    row->addWidget(previousButton());
    row->addWidget(nextButton());
    row->addWidget(toggleEmbedModeButton());
    row->addWidget(toggleMiniModeButton());
    row->addWidget(toggleFullScreenModeButton());
    row->addWidget(toggleTraceWindowButton());
    row->addWidget(volumeSlider());
    row->addWidget(positionButton());
    row->addWidget(progressButton());

    row->addStretch();

    row->addWidget(networkButton());
    row->addWidget(userButton());
    row->addWidget(prefixComboBox());

    OverlayLayout *input = new OverlayLayout;
    input->addWidget(inputComboBox());
    input->addWidget(inputCountButton(), Qt::AlignRight);
    input->setContentsMargins(0, 0, 0, 0);
    row->addLayout(input);

    //row->addStretch();
    //row->addWidget(volumeSlider());
    //row->addWidget(positionButton());
    //row->addWidget(progressButton());

    // left, top, right, bottom
    rows->setContentsMargins(9, 4, 9, 4);
    rows->setSpacing(1);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(rows);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

#ifndef WITH_WIN_PICKER
  toggleTraceWindowButton()->hide();
  toggleTraceWindowButton()->resize(QSize());
#endif // WITH_WIN_PICKER
  setStyleSheet(styleSheet() + SS_PLAYER);
}

// - Properties -

void
EmbeddedPlayerUi::setFullScreenMode(bool t)
{
  if (fullScreen_ != t) {
    fullScreen_ = t;
    updateTrackingTimer();
    updateGeometry();
    emit fullScreenModeChanged(fullScreen_);
  }
}

void
EmbeddedPlayerUi::setOnTop(bool t)
{
  if (top_ != t) {
    top_ = t;
    canvas_->setVisible(!top_);
    infoView_->setVisible(!top_);
    updateDockButton();
    updateGeometry();
  }
}

// - Auto hide -

void
EmbeddedPlayerUi::autoHide()
{
  if (underMouse() ||
      inputComboBox()->hasFocus() || prefixComboBox()->hasFocus()
#ifndef Q_OS_WIN
      // Always keep embedded player visible in live mode
      // Because mouse hook is missing in non-windows platform.
      || (hub()->isLiveTokenMode() && hub()->isEmbeddedPlayerMode())
#endif // Q_OS_WIN
      )
    resetAutoHideTimeout();
  else if (isVisible()) {
    hide();
    if (canvas_->isVisible())
      canvas_->hide();
  }
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
  //if (autoHideTimer_->isActive())
  //  autoHideTimer_->stop();
  //autoHideTimer_->start();
  QTimer::singleShot(0, autoHideTimer_, SLOT(start()));
}

// - Geometry -

void
EmbeddedPlayerUi::updateGeometry()
{
  if (fullScreen_ && QApplication::desktop()) {
    QRect r = QApplication::desktop()->screenGeometry(this);
    Q_ASSERT(!r.isEmpty());

    // Update size
    int w_max = r.width(),
        h_hint = sizeHint().height();
    QSize newSize(w_max, h_hint);
    if (newSize != size())
      resize(newSize);

    int x_left = r.x(),
        y = top_ ? r.y() :
                   r.y() + r.height() - height();
    moveToGlobalPos(QPoint(x_left, y));

  } else if (containerWindow_) {
#ifdef Q_OS_WIN
    QRect r = QtWin::getWindowRect(containerWindow_);
    if (r.isEmpty()) {
      //if (!QtWin::isWindowValid(containerWindow_))
      setContainerWindow(nullptr);

    } else if (r.topLeft() == QTWIN_INVALID_POS) {
      //if (QtWin::isWindowMinimized(containerWindow_))
      //  TODO: minimize();
    } else {
      int w_max = r.width(),
          h_hint = sizeHint().height();
      QSize newSize(w_max, h_hint);
      if (newSize != size())
        resize(newSize);

      //int window_header_height = hub()->isSignalTokenMode() ? 40 : 0;
      //enum { window_header_height = 40 };
      enum { window_header_height = 0 };

      int x_left = r.x();
      int y = top_ ? r.top() + window_header_height : r.bottom() - height();

      moveToGlobalPos(QPoint(x_left, y));
    }
#endif // Q_OS_WIN
  } else {
    QWidget *w = containerWidget_ ? containerWidget_ : parentWidget();
    if(w) {
      // Invalidate size
      int w_max = w->width(),
          h_hint = sizeHint().height();
      QSize newSize(w_max, h_hint);
      if (newSize != size())
        resize(newSize);

      // Invalidate position
      //int x_center = widget->width() / 2 - 2 * width();
      QPoint g = w->mapToGlobal(QPoint());
      int x_left = g.x(),
          y = top_ ? g.y() :
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
    updateTrackingTimer();
  }
}

void
EmbeddedPlayerUi::setContainerWidget(QWidget *w)
{
  if (containerWidget_ != w) {
    containerWidget_ = w;
    updateTrackingTimer();
  }
}

void
EmbeddedPlayerUi::updateTrackingTimer()
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

  if (visible) {
    canvas_->updateVisible();
    calibration_->updateVisible();
    infoView_->refresh();
    updateGeometry();
  }

  Base::setVisible(visible);
  updateTrackingTimer();
  emit visibleChanged(visible);
}

void
EmbeddedPlayerUi::hideInfoView(bool invisible)
{
  bool visible = !top_ && !invisible;
  infoView_->setVisible(visible);
}

void
EmbeddedPlayerUi::showWhenEmbedded()
{
  if (!isVisible() && hub()->isEmbeddedPlayerMode())
    show();
}

void
EmbeddedPlayerUi::updateDockButton()
{
  toggleDockButton_->setChecked(true);
  if (top_) {
    toggleDockButton_->setStyleSheet(SS_TOOLBUTTON_DOCK_BOTTOM);
    toggleDockButton_->setToolTip(TR(T_TOOLTIP_DOCK_BOTTOM));
  } else {
    toggleDockButton_->setStyleSheet(SS_TOOLBUTTON_DOCK_TOP);
    toggleDockButton_->setToolTip(TR(T_TOOLTIP_DOCK_TOP));
  }
}

void
EmbeddedPlayerUi::updateInputCountButton()
{
#ifdef Q_OS_MAC
# define PADDING "    "
#else
# define PADDING "   "
#endif // Q_OS_MAC
  Base::updateInputCountButton();
  QToolButton *b = inputCountButton();
  b->setText(b->text() + PADDING); // padding
#undef PADDING
}

// EOF
