// ac/acui.cc
// 7/30/2011

#include "project/common/acui.h"
#include "project/common/acrc.h"
#include "project/common/acss.h"
#include "project/common/accomboedit.h"
#include "project/common/actextedit.h"
#include "project/common/aclineedit.h"
#include "module/qtext/combobox.h"
#include "module/qtext/toolbutton.h"
#ifdef WITH_WIN_DWM
#  include "win/dwm/dwm.h"
#endif // WITH_WIN_DWM
#ifdef Q_WS_WIN
#  include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#include <QtGui>

// - Constructions -

AcUi::AcUi(QObject *parent)
  : Base(parent), menu_(false), theme_(DefaultTheme)
{
#ifdef Q_WS_WIN
  aero_ = true;
#else
  aero_ = false;
#endif // Q_WS_WIN
}

void
AcUi::setTheme(int tid)
{
  theme_ = (Theme)tid;
  updateBackground();
}

const char*
AcUi::backgroundImagePath() const
{
  int t = theme_;
  if (t == RandomTheme)
    t = ::qrand() % (ThemeCount - 2) + 2; // eliminate first 2 themes (default and random)

  const char *rc = 0;
  switch (t) {
  case DarkTheme:       rc = ACRC_IMAGE_AERO_DARK; break;
  case BlackTheme:      rc = ACRC_IMAGE_AERO_BLACK; break;
  case BlueTheme:       rc = ACRC_IMAGE_AERO_BLUE; break;
  case BrownTheme:      rc = ACRC_IMAGE_AERO_BROWN; break;
  case CyanTheme:       rc = ACRC_IMAGE_AERO_CYAN; break;
  case GrayTheme:       rc = ACRC_IMAGE_AERO_GRAY; break;
  case GreenTheme:      rc = ACRC_IMAGE_AERO_GREEN; break;
  case PinkTheme:       rc = ACRC_IMAGE_AERO_PINK; break;
  case PurpleTheme:     rc = ACRC_IMAGE_AERO_PURPLE; break;
  case RedTheme:        rc = ACRC_IMAGE_AERO_RED; break;
  case WhiteTheme:      rc = ACRC_IMAGE_AERO_WHITE; break;
  case YellowTheme:     rc = ACRC_IMAGE_AERO_YELLOW; break;

  case DefaultTheme:
  case RandomTheme:
  default: rc = ACRC_IMAGE_AERO;
  }

  return rc;
}

bool
AcUi::isAeroAvailable()
{
  return
#ifdef Q_WS_WIN
  QtWin::isWindowsVistaOrLater()
#if WITH_WIN_DWM
  && Dwm::isCompositionEnabled()
#endif // WITH_WIN_DWM
#else
  false
#endif // Q_WS_WIN;
  ;
}

// - Styles -

void
AcUi::setMainWindowStyle(QWidget *w)
{
  Q_ASSERT(w);
#ifdef WITH_WIN_DWM
  if(isAeroEnabled()) {
    w->setAttribute(Qt::WA_TranslucentBackground);
    w->setAttribute(Qt::WA_NoSystemBackground);

    setWindowDwmEnabled(w, true);
    return;
  }
#endif // WITH_WIN_DWM
  //if (qss)
  //  w->setStyleSheet(SS_BACKGROUND_CLASS(MainWindow));
  setWindowBackground(w, true); // persistent = true
  w->setWindowOpacity(AC_WINDOW_OPACITY);
}

void
AcUi::setBlackBackground(QWidget *w)
{
  Q_ASSERT(w);
  if (w) {
    w->ensurePolished();
    w->setPalette(Qt::black);
  }
}

void
AcUi::setWindowBackground(QWidget *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w)
    return;

  const char *rc = backgroundImagePath();

  QPalette palette;
  palette.setBrush(QPalette::Window, QPixmap(rc));

  w->ensurePolished(); // must appear before setPalette to flush window manager, or system theme would override backgroud
  w->setPalette(palette);

  if (persistent) {
    if (!windows_.contains(w))
      windows_.append(w);
  }
}

void
AcUi::setMenuBackground(QMenu *m, bool persistent)
{
  Q_ASSERT(m);
  if (!m || !menu_)
    return;

//#ifdef Q_WS_MAC
  if (theme_ == DefaultTheme)
    m->setStyleSheet(QString());
  else
//#endif // Q_WS_MAC
  {
    QString rc = backgroundImagePath();
    m->setStyleSheet(
      SS_CONTEXTMENU
      SS_BEGIN(QMenu)
        SS_BACKGROUND_IMAGE_URL(+rc+)
      SS_END
    );
  }

  if (persistent) {
    if (!menus_.contains(m))
      menus_.append(m);
  }
}

void
AcUi::updateBackground()
{
  if (!windows_.isEmpty())
    foreach (QWidget *w, windows_)
      setWindowBackground(w, false); // persistent = false

  if (menu_ && !menus_.isEmpty())
    foreach (QMenu *m, menus_)
      setMenuBackground(m, false); // persistent = false
}

#ifdef WITH_WIN_DWM
void
AcUi::setWindowDwmEnabled(QWidget *w, bool t)
{
  if (t) {
    Dwm::enableBlurBehindWindow(w, true);
    Dwm::extendFrameIntoClientArea(w, -1, -1, -1, -1);
  } else {
    Dwm::enableBlurBehindWindow(w, false);
    Dwm::extendFrameIntoClientArea(w, 0, 0, 0, 0);
  }
}

void
AcUi::setWindowDwmEnabled(WId wid, bool t)
{
  if (t) {
    Dwm::enableBlurBehindWindow(wid, true);
    Dwm::extendFrameIntoClientArea(wid, -1, -1, -1, -1);
  } else {
    Dwm::enableBlurBehindWindow(wid, false);
    Dwm::extendFrameIntoClientArea(wid, 0, 0, 0, 0);
  }
}

void
AcUi::setDwmEnabled(bool t)
{
  foreach (QWidget *w, dwmEnabledWindows_)
    if (t)
      DWM_ENABLE_AERO_WIDGET(w)
    else
      DWM_DISABLE_AERO_WIDGET(w)
}
#endif // WITH_WIN_DWM

void
AcUi::setWindowStyle(QWidget *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w)
    return;

  //w->setGraphicsEffect(new QGraphicsBlurEffect(w));

#ifdef WITH_WIN_DWM
  if (isAeroEnabled()) {
    if (persistent) {
      DWM_ENABLE_AERO_WIDGET(w)
      dwmEnabledWindows_.append(w);
    } else
      DWM_ENABLE_ONETIME_AERO_WIDGET(w)
    return;

    //if (qss) {
    //  if(Dwm::isCompositionEnabled())
    //    w->setStyleSheet(SS_WINDOW_DWM);
    //  else
    //    w->setStyleSheet(SS_WINDOW);
    //}
  }
#endif // WITH_WIN_DWM

  //w->setAttribute(Qt::WA_TranslucentBackground);
  //w->setAttribute(Qt::WA_NoSystemBackground, false);
  //w->setAttribute(Qt::WA_StyledBackground, false);
  //w->setAutoFillBackground(true);

  setWindowBackground(w, persistent);
  w->setWindowOpacity(AC_WINDOW_OPACITY);

  //if (qss)
  //  w->setStyleSheet(SS_WINDOW);

  /*
#ifdef Q_WS_X11
  w->setAttribute(Qt::WA_TranslucentBackground);
  w->setAttribute(Qt::WA_NoSystemBackground, false);
  w->setAttribute(Qt::WA_StyledBackground, false);
  QPalette pal = w->palette();
  QColor bg = pal.window().color();
  bg.setAlpha(AC_WINDOW_ALPHA);
  pal.setColor(QPalette::Window, bg);
  w->setPalette(pal);
  w->ensurePolished(); // workaround Oxygen filling the background
#endif // Q_WS_X11
  */
}

/*
void
AcUi::setMenuStyle(QMenu *menu)
{
  Q_ASSERT(menu);
  if (!menu)
    return;

#ifdef WITH_WIN_DWM
  if (isAeroEnabled()) {
    DWM_ENABLE_AERO_WIDGET(menu);
    return;
  }
#endif // WITH_WIN_DWM

  menu->setStyleSheet(SS_MENU);
}
*/

void
AcUi::setContextMenuStyle(QMenu *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w || !menu_)
    return;

#ifdef WITH_WIN_DWM
  if (isAeroEnabled()) {
    if (persistent)
      DWM_ENABLE_AERO_WIDGET(w)
    else
      DWM_ENABLE_ONETIME_AERO_WIDGET(w)
    //if (qss)
    //  w->setStyleSheet(SS_CONTEXTMENU_DWM);

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(w);
    effect->setColor(AC_CONTEXTMENU_COLOR);
    effect->setStrength(AC_CONTEXTMENU_COLOR_STRENGTH);
    w->setGraphicsEffect(effect);

    return;
  }
#endif // WITH_WIN_DWM

  setMenuBackground(w, persistent);
  w->setWindowOpacity(AC_CONTEXTMENU_OPACITY);

  //QGraphicsOpacityEffect *transparent = new QGraphicsOpacityEffect(w);
  //w->setGraphicsEffect(transparent);

  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(w);
  //w->setGraphicsEffect(blur);
}

void
AcUi::setToolButtonStyle(QToolButton *button)
{
  Q_ASSERT(button);
  if (!button)
    return;

#ifdef Q_WS_WIN
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
  blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  blur->setBlurRadius(AC_TOOLBUTTON_BLUR_RADIUS);
  button->setGraphicsEffect(blur);
#endif // Q_WS_WIN
}

void
AcUi::setStatusBarStyle(QStatusBar *w)
{
  Q_ASSERT(w);
  if (!w)
    return;
#ifdef WITH_WIN_DWM
  if (isAeroEnabled())
    w->setStyleSheet(SS_STATUSBAR_DWM);
#endif // WITH_WIN_DWM
}

// - Maker

QToolButton*
AcUi::makeToolButton(ulong hints, const QString &title, const QString &tip, const QString &key,
                        QObject *receiver, const char *slot, Qt::ConnectionType type)
{
  QToolButton *ret = new QtExt::ToolButton;
  ret->setToolButtonStyle(Qt::ToolButtonTextOnly);

  ret->setStyleSheet(
        hints & InvertHint ? SS_TOOLBUTTON_TEXT_INVERT :
        hints & HighlightHint ? SS_TOOLBUTTON_TEXT_HIGHLIGHT :
        hints & PushHint ? SS_TOOLBUTTON_TEXT_NORMAL :
        hints & CheckHint ? SS_TOOLBUTTON_TEXT_CHECKABLE :
        hints & TabHint ? SS_TOOLBUTTON_TEXT_TAB :
        hints & UrlHint ? SS_TOOLBUTTON_TEXT_URL :
        SS_TOOLBUTTON_TEXT
  );

  if (!title.isEmpty())
    ret->setText(
        hints & PushHint ? QString("[ %1 ]").arg(title) :
        hints & CheckHint ? QString("| %1 |").arg(title) :
        hints & TabHint ? QString("- %1 -").arg(title) :
        hints & BuddyHint ? title + ":" :
        hints & MenuHint ? title + " " :
        title
    );

  QString t = tip.isEmpty() ? title : tip;
  if (!key.isEmpty())
    t += " [" + key + "]";
  ret->setToolTip(t);

  if (hints & (CheckHint | TabHint | UrlHint))
    ret->setCheckable(true);
  if (hints & UrlHint)
    ret->setChecked(true);

  if (receiver && slot) {
    if (hints & (CheckHint | TabHint | UrlHint))
      connect(ret, SIGNAL(clicked(bool)), receiver, slot, type);
    else
      connect(ret, SIGNAL(clicked()), receiver, slot, type);
  } else if (hints & MenuHint)
    connect(ret, SIGNAL(clicked()), ret, SLOT(showMenu()));

  return ret;
}

QLabel*
AcUi::makeLabel(ulong hints, const QString &title, const QString &tip, QWidget *buddy)
{
  QLabel *ret = new QLabel;

  ret->setStyleSheet(hints & HighlightHint ? SS_LABEL_HIGHLIGHT :
                     hints & UrlHint ? SS_LABEL_URL :
                     SS_LABEL);
  ret->setText(hints & BuddyHint ? title + ":" : title);
  ret->setToolTip(tip.isEmpty() ? title : tip);

  if (buddy)
    ret->setBuddy(buddy);

  return ret;
}

QRadioButton*
AcUi::makeRadioButton(ulong hints, const QString &title, const QString &tip)
{
  Q_UNUSED(hints);
  QRadioButton *ret = new QRadioButton;

  ret->setStyleSheet(SS_RADIOBUTTON_TEXT);
  ret->setText(title);
  ret->setToolTip(tip.isEmpty() ? title : tip);

  return ret;
}

QTextEdit*
AcUi::makeTextEdit(ulong hints, const QString &tip)
{
  QTextEdit *ret = new AcTextEdit;
  ret->setToolTip(tip);
  ret->setReadOnly(hints & ReadOnlyHint);

  return ret;
}

QComboBox*
AcUi::makeComboBox(ulong hints, const QString &text, const QString &tip, const QString &holder, const QStringList &items)
{
  QComboBox *ret;
  if (hints | EditHint)
    ret = new AcComboEdit(items);
  else {
    ret = new QtExt::ComboBox;
    ret->setStyleSheet(SS_COMBOBOX);
    ret->addItems(items);
  }

  if (!text.isEmpty())
    ret->setEditText(text);
  ret->setToolTip(tip);
  ret->lineEdit()->setPlaceholderText(holder);
  ret->setEditable(!(hints & ReadOnlyHint));

  return ret;
}

QLineEdit*
AcUi::makeLineEdit(ulong hints, const QString &text, const QString &tip, const QString &holder)
{
  QLineEdit *ret = new AcLineEdit;
  ret->setText(text);
  ret->setToolTip(tip);
  ret->setPlaceholderText(holder);
  ret->setReadOnly(hints & ReadOnlyHint);
  if (hints & PasswordHint)
    ret->setEchoMode(QLineEdit::Password);
  return ret;
}

// EOF
