// uistyle.cc
// 7/30/2011

#include "uistyle.h"
#include "global.h"
#include "stylesheet.h"
#ifdef USE_WIN_DWM
  #include "win/dwm/dwm.h"
#endif // USE_WIN_DWM
#ifdef USE_WIN_QTWIN
  #include "win/qtwin/qtwin.h"
#endif USE_WIN_QTWIN
#include <QtGui>

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

bool
UiStyle::isAeroAvailable()
{
  return false;
  return
#ifdef Q_WS_WIN
  true
#ifdef USE_WIN_QTWIN
  && QtWin::isWindowsVistaOrLater()
#endif // USE_WIN_QTWIN
#if USE_WIN_DWM
  && Dwm::isCompositionEnabled()
#endif // USE_WIN_DWM
#else
  false
#endif Q_WS_WIN;
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
  setBackground(w, true); // persistent = true
#ifdef Q_WS_WIN
  w->setWindowOpacity(G_WINDOW_OPACITY);
#endif // Q_WS_WIN
}

void
UiStyle::setBackground(QWidget *w, bool persistent)
{
  Q_ASSERT(w);
  if (!w)
    return;

  int t = theme_;
  if (t == RandomTheme)
    t = ::qrand() % (ThemeCount - 2) + 2; // eliminate first 2 themes (default and random)

  const char *rc = 0;
  switch (t) {
  case BlackTheme1:     rc = RC_IMAGE_AERO_BLACK1; break;
  case BlackTheme2:     rc = RC_IMAGE_AERO_BLACK2; break;
  case BlueTheme1:      rc = RC_IMAGE_AERO_BLUE1; break;
  case BlueTheme2:      rc = RC_IMAGE_AERO_BLUE2; break;
  case BrownTheme1:     rc = RC_IMAGE_AERO_BROWN1; break;
  case BrownTheme2:     rc = RC_IMAGE_AERO_BROWN2; break;
  case GreenTheme1:     rc = RC_IMAGE_AERO_GREEN1; break;
  case GreenTheme2:     rc = RC_IMAGE_AERO_GREEN2; break;
  case LightBlueTheme1: rc = RC_IMAGE_AERO_LIGHTBLUE1; break;
  case LightBlueTheme2: rc = RC_IMAGE_AERO_LIGHTBLUE2; break;
  case OrangeTheme1:    rc = RC_IMAGE_AERO_ORANGE1; break;
  case OrangeTheme2:    rc = RC_IMAGE_AERO_ORANGE2; break;
  case PinkTheme1:      rc = RC_IMAGE_AERO_PINK1; break;
  case PinkTheme2:      rc = RC_IMAGE_AERO_PINK2; break;
  case PurpleTheme1:    rc = RC_IMAGE_AERO_PURPLE1; break;
  case PurpleTheme2:    rc = RC_IMAGE_AERO_PURPLE2; break;
  case RedTheme1:       rc = RC_IMAGE_AERO_RED1; break;
  case RedTheme2:       rc = RC_IMAGE_AERO_RED2; break;
  case YellowTheme1:    rc = RC_IMAGE_AERO_YELLOW1; break;
  case YellowTheme2:    rc = RC_IMAGE_AERO_YELLOW2; break;
  case WindowsTheme1:   rc = RC_IMAGE_AERO_WIN1; break;
  case WindowsTheme2:   rc = RC_IMAGE_AERO_WIN2; break;

  case DefaultTheme:
  case RandomTheme:
  default: rc = RC_IMAGE_AERO;
  }

  QPalette palette;
  palette.setBrush(QPalette::Window, QPixmap(rc));

  w->ensurePolished(); // must appear before setPalette to flush window manager, or system theme would override backgroud
  w->setPalette(palette);

  if (persistent) {
    if (!widgets_.contains(w))
      widgets_.append(w);
  }
}

void
UiStyle::invalidateBackground()
{
  if (!widgets_.isEmpty())
    foreach (QWidget *w, widgets_)
      setBackground(w, false); // persistent = false
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
    DWM_ENABLE_AERO_WIDGET(w);
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

  setBackground(w, persistent);
#ifdef Q_WS_WIN
  w->setWindowOpacity(G_WINDOW_OPACITY);
#endif // Q_WS_WIN

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
UiStyle::setContextMenuStyle(QMenu *menu, bool persistent)
{
  Q_ASSERT(menu);
  if (!menu)
    return;

#ifdef USE_WIN_DWM
  if (isAeroAvailable()) {
    DWM_ENABLE_ONETIME_AERO_WIDGET(menu);
    //if (qss)
    //  menu->setStyleSheet(SS_CONTEXTMENU_DWM);

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(menu);
    effect->setColor(G_CONTEXTMENU_COLOR);
    effect->setStrength(G_CONTEXTMENU_COLOR_STRENGTH);
    menu->setGraphicsEffect(effect);

    return;
  }
#endif // USE_WIN_DWM

#ifdef Q_WS_WIN
  // Window XP
  menu->setStyleSheet(SS_CONTEXTMENU);
  setBackground(menu, persistent); // persistent = false

  //QGraphicsOpacityEffect *transparent = new QGraphicsOpacityEffect(menu);
  //menu->setGraphicsEffect(transparent);
  menu->setWindowOpacity(G_CONTEXTMENU_OPACITY);

  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(menu);
  //menu->setGraphicsEffect(blur);

#else
  // Do nothing for Mac/Linux
  //if (qss)
  //  menu->setStyleSheet(SS_CONTEXTMENU);
  Q_UNUSED(persistent);
#endif // Q_WS_WIN
}

void
UiStyle::setToolButtonStyle(QToolButton *button, bool qss)
{
  Q_ASSERT(button);
  if (!button)
    return;

  Q_UNUSED(qss);

#ifdef Q_WS_WIN
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
  blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
  blur->setBlurRadius(G_TOOLBUTTON_BLUR_RADIUS);
  button->setGraphicsEffect(blur);
#endif // Q_WS_WIN
}

void
UiStyle::setComboBoxStyle(QComboBox *w, bool qss)
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

  if (qss)
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
