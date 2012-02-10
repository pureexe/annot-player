// uistyle.cc
// 7/30/2011

#include "uistyle.h"
#include "defines.h"
#include "stylesheet.h"
#ifdef USE_WIN_DWM
  #include "win/dwm/dwm.h"
#endif // USE_WIN_DWM
#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
#include <QtGui>

//#define USE_THEME_MENU

// - Constructions -

namespace { UiStyle default_; }
UiStyle *UiStyle::global_ = &default_;

UiStyle::UiStyle(QObject *parent) : Base(parent), theme_(DefaultTheme) { }

UiStyle::Theme
UiStyle::theme() const
{ return theme_; }

void
UiStyle::setTheme(int tid)
{
  theme_ = (Theme)tid;
  invalidateBackground();
}

const char*
UiStyle::backgroundImagePath() const
{
  int t = theme_;
  if (t == RandomTheme)
    t = ::qrand() % (ThemeCount - 2) + 2; // eliminate first 2 themes (default and random)

  const char *rc = 0;
  switch (t) {
  case DarkTheme:       rc = RC_IMAGE_AERO_DARK; break;
  case BlackTheme:      rc = RC_IMAGE_AERO_BLACK; break;
  case BlueTheme:       rc = RC_IMAGE_AERO_BLUE; break;
  case BrownTheme:      rc = RC_IMAGE_AERO_BROWN; break;
  case CyanTheme:       rc = RC_IMAGE_AERO_CYAN; break;
  case GrayTheme:       rc = RC_IMAGE_AERO_GRAY; break;
  case GreenTheme:      rc = RC_IMAGE_AERO_GREEN; break;
  case PinkTheme:       rc = RC_IMAGE_AERO_PINK; break;
  case PurpleTheme:     rc = RC_IMAGE_AERO_PURPLE; break;
  case RedTheme:        rc = RC_IMAGE_AERO_RED; break;
  case WhiteTheme:      rc = RC_IMAGE_AERO_WHITE; break;
  case YellowTheme:     rc = RC_IMAGE_AERO_YELLOW; break;

  case DefaultTheme:
  case RandomTheme:
  default: rc = RC_IMAGE_AERO;
  }

  return rc;
}

bool
UiStyle::isAeroAvailable()
{
  return
#ifdef Q_WS_WIN
  QtWin::isWindowsVistaOrLater()
#if USE_WIN_DWM
  && Dwm::isCompositionEnabled()
#endif // USE_WIN_DWM
#else
  false
#endif // Q_WS_WIN;
  ;
}

// - Styles -

void
UiStyle::setMainWindowStyle(QWidget *w)
{
  Q_ASSERT(w);
#ifdef USE_WIN_DWM
  if(isAeroAvailable()) {
    w->setAttribute(Qt::WA_TranslucentBackground);
    w->setAttribute(Qt::WA_NoSystemBackground);

    setWindowDwmEnabled(w, true);
    return;
  }
#endif // USE_WIN_DWM
  //if (qss)
  //  w->setStyleSheet(SS_BACKGROUND_CLASS(MainWindow));
  setWindowBackground(w, true); // persistent = true
  w->setWindowOpacity(G_WINDOW_OPACITY);
}

void
UiStyle::setBlackBackground(QWidget *w)
{
  Q_ASSERT(w);
  if (w) {
    w->ensurePolished();
    w->setPalette(QColor("black"));
  }
}

void
UiStyle::setWindowBackground(QWidget *w, bool persistent)
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
UiStyle::setMenuBackground(QMenu *m, bool persistent)
{
#ifdef USE_THEME_MENU
  Q_ASSERT(m);
  if (!m)
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
        SS_BACKGROUND_IMAGE_URL_BEGIN
          + rc +
        SS_BACKGROUND_IMAGE_URL_END
      SS_END
    );
  }

  if (persistent) {
    if (!menus_.contains(m))
      menus_.append(m);
  }
#else
  Q_UNUSED(m);
  Q_UNUSED(persistent);
#endif // USE_THEME_MENU
}

void
UiStyle::invalidateBackground()
{
  if (!windows_.isEmpty())
    foreach (QWidget *w, windows_)
      setWindowBackground(w, false); // persistent = false

#ifdef USE_THEME_MENU
  if (!menus_.isEmpty())
    foreach (QMenu *m, menus_)
      setMenuBackground(m, false); // persistent = false
#endif // USE_THEME_MENU
}

#ifdef USE_WIN_DWM
void
UiStyle::setWindowDwmEnabled(QWidget *w, bool t)
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
UiStyle::setWindowDwmEnabled(WId wid, bool t)
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
UiStyle::setDwmEnabled(bool t)
{
  foreach (QWidget *w, dwmEnabledWindows_)
    if (t)
      DWM_ENABLE_AERO_WIDGET(w)
    else
      DWM_DISABLE_AERO_WIDGET(w)
}
#endif // USE_WIN_DWM

void
UiStyle::setWindowStyle(QWidget *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w)
    return;

  //w->setGraphicsEffect(new QGraphicsBlurEffect(w));

#ifdef USE_WIN_DWM
  if (isAeroAvailable()) {
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
#endif // USE_WIN_DWM

  //w->setAttribute(Qt::WA_TranslucentBackground);
  //w->setAttribute(Qt::WA_NoSystemBackground, false);
  //w->setAttribute(Qt::WA_StyledBackground, false);
  //w->setAutoFillBackground(true);

  setWindowBackground(w, persistent);
  w->setWindowOpacity(G_WINDOW_OPACITY);

  //if (qss)
  //  w->setStyleSheet(SS_WINDOW);

  /*
#ifdef Q_WS_X11
  w->setAttribute(Qt::WA_TranslucentBackground);
  w->setAttribute(Qt::WA_NoSystemBackground, false);
  w->setAttribute(Qt::WA_StyledBackground, false);
  QPalette pal = w->palette();
  QColor bg = pal.window().color();
  bg.setAlpha(G_WINDOW_ALPHA);
  pal.setColor(QPalette::Window, bg);
  w->setPalette(pal);
  w->ensurePolished(); // workaround Oxygen filling the background
#endif // Q_WS_X11
  */
}

/*
void
UiStyle::setMenuStyle(QMenu *menu)
{
  Q_ASSERT(menu);
  if (!menu)
    return;

#ifdef USE_WIN_DWM
  if (isAeroAvailable()) {
    DWM_ENABLE_AERO_WIDGET(menu);
    return;
  }
#endif // USE_WIN_DWM

  menu->setStyleSheet(SS_MENU);
}
*/

void
UiStyle::setContextMenuStyle(QMenu *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w)
    return;

#ifdef USE_WIN_DWM
  if (isAeroAvailable()) {
    if (persistent)
      DWM_ENABLE_AERO_WIDGET(w)
    else
      DWM_ENABLE_ONETIME_AERO_WIDGET(w)
    //if (qss)
    //  w->setStyleSheet(SS_CONTEXTMENU_DWM);

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(w);
    effect->setColor(G_CONTEXTMENU_COLOR);
    effect->setStrength(G_CONTEXTMENU_COLOR_STRENGTH);
    w->setGraphicsEffect(effect);

    return;
  }
#endif // USE_WIN_DWM

  setMenuBackground(w, persistent);
  w->setWindowOpacity(G_CONTEXTMENU_OPACITY);

  //QGraphicsOpacityEffect *transparent = new QGraphicsOpacityEffect(w);
  //w->setGraphicsEffect(transparent);

  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(w);
  //w->setGraphicsEffect(blur);
}

void
UiStyle::setToolButtonStyle(QToolButton *button)
{
  Q_ASSERT(button);
  if (!button)
    return;

//#ifdef Q_WS_WIN
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
  blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  blur->setBlurRadius(G_TOOLBUTTON_BLUR_RADIUS);
  button->setGraphicsEffect(blur);
//#endif // Q_WS_WIN
}

void
UiStyle::setComboBoxStyle(QComboBox *w)
{
  Q_ASSERT(w);
  if (!w)
    return;

  /*
#ifdef USE_WIN_DWM
  DWM_ENABLE_ONETIME_AERO_WIDGET(w);
  if (w->view())
    DWM_ENABLE_ONETIME_AERO_WIDGET(w->view());
#endif
  */

  w->setStyleSheet(SS_COMBOBOX);

  /*
#ifdef Q_WS_WIN
  if (w->view()) {
    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(w);
    effect->setColor(G_CONTEXTMENU_COLOR);
    effect->setStrength(G_CONTEXTMENU_COLOR_STRENGTH);
    w->view()->setGraphicsEffect(effect);
  }
#endif // Q_WS_WIN
  */
}

// EOF
