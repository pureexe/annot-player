// uistyle.cc
// 7/30/2011

#include "uistyle.h"
#include "global.h"
#include "stylesheet.h"
#ifdef USE_WIN_DWM
  #include "win/dwm/dwm.h"
#endif // USE_WIN_DWM
#ifdef Q_WS_WIN
  #include "win/qtwin/qtwin.h"
#endif // Q_WS_WIN
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

const char*
UiStyle::backgroundImagePath() const
{
  int t = theme_;
  if (t == RandomTheme)
    t = ::qrand() % (ThemeCount - 2) + 2; // eliminate first 2 themes (default and random)

  const char *rc = 0;
  switch (t) {
  case Black1Theme:     rc = RC_IMAGE_AERO_BLACK1; break;
  case Black2Theme:     rc = RC_IMAGE_AERO_BLACK2; break;
  case Blue1Theme:      rc = RC_IMAGE_AERO_BLUE1; break;
  case Blue2Theme:      rc = RC_IMAGE_AERO_BLUE2; break;
  case Brown1Theme:     rc = RC_IMAGE_AERO_BROWN1; break;
  case Brown2Theme:     rc = RC_IMAGE_AERO_BROWN2; break;
  case Green1Theme:     rc = RC_IMAGE_AERO_GREEN1; break;
  case Green2Theme:     rc = RC_IMAGE_AERO_GREEN2; break;
  case LightBlue1Theme: rc = RC_IMAGE_AERO_LIGHTBLUE1; break;
  case LightBlue2Theme: rc = RC_IMAGE_AERO_LIGHTBLUE2; break;
  case Orange1Theme:    rc = RC_IMAGE_AERO_ORANGE1; break;
  case Orange2Theme:    rc = RC_IMAGE_AERO_ORANGE2; break;
  case Pink1Theme:      rc = RC_IMAGE_AERO_PINK1; break;
  case Pink2Theme:      rc = RC_IMAGE_AERO_PINK2; break;
  case Purple1Theme:    rc = RC_IMAGE_AERO_PURPLE1; break;
  case Purple2Theme:    rc = RC_IMAGE_AERO_PURPLE2; break;
  case Red1Theme:       rc = RC_IMAGE_AERO_RED1; break;
  case Red2Theme:       rc = RC_IMAGE_AERO_RED2; break;
  case Yellow1Theme:    rc = RC_IMAGE_AERO_YELLOW1; break;
  case Yellow2Theme:    rc = RC_IMAGE_AERO_YELLOW2; break;
  case Windows1Theme:   rc = RC_IMAGE_AERO_WIN1; break;
  case Windows2Theme:   rc = RC_IMAGE_AERO_WIN2; break;

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
}

void
UiStyle::invalidateBackground()
{
  if (!windows_.isEmpty())
    foreach (QWidget *w, windows_)
      setWindowBackground(w, false); // persistent = false

  if (!menus_.isEmpty())
    foreach (QMenu *m, menus_)
      setMenuBackground(m, false); // persistent = false
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
UiStyle::setContextMenuStyle(QMenu *m, bool persistent)
{
  Q_ASSERT(m);
  if (!m)
    return;

#ifdef USE_WIN_DWM
  if (isAeroAvailable()) {
    DWM_ENABLE_ONETIME_AERO_WIDGET(m);
    //if (qss)
    //  m->setStyleSheet(SS_CONTEXTMENU_DWM);

    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(m);
    effect->setColor(G_CONTEXTMENU_COLOR);
    effect->setStrength(G_CONTEXTMENU_COLOR_STRENGTH);
    m->setGraphicsEffect(effect);

    return;
  }
#endif // USE_WIN_DWM

  setMenuBackground(m, persistent);
  m->setWindowOpacity(G_CONTEXTMENU_OPACITY);

  //QGraphicsOpacityEffect *transparent = new QGraphicsOpacityEffect(m);
  //m->setGraphicsEffect(transparent);

  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(m);
  //m->setGraphicsEffect(blur);
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
