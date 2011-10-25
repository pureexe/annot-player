// uistyle.cc
// 7/30/2011

#include "uistyle.h"
#include "global.h"
#include "stylesheet.h"
#ifdef USE_WIN_DWM
  #include "win/dwm/dwm.h"
#endif // USE_WIN_DWM
#include <QtGui>

// - Constructions -

namespace { UiStyle default_; }
UiStyle *UiStyle::global_ = &default_;

UiStyle::UiStyle(QObject *parent) : Base(parent) { }

// - Styles -

void
UiStyle::setMainWindowStyle(QWidget *w, bool qss)
{
  Q_ASSERT(w);
  if (qss)
    w->setStyleSheet(SS_MAINWINDOW);
#ifndef USE_WIN_DWM
  Q_UNUSED(w);
#else
  w->setAttribute(Qt::WA_TranslucentBackground);
  w->setAttribute(Qt::WA_NoSystemBackground);

  setWindowDwmEnabled(w, true);
#endif // USE_WIN_DWM
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
UiStyle::setWindowStyle(QWidget *w, bool qss)
{
  Q_ASSERT(w);
  if (!w)
    return;

  //w->setGraphicsEffect(new QGraphicsBlurEffect(w));

#ifdef USE_WIN_DWM
  DWM_ENABLE_AERO_WIDGET(w);
  if (Dwm::isCompositionEnabled()) {
    if (qss)
      w->setStyleSheet(SS_WINDOW_DWM);
    return;
  }
#endif // USE_WIN_DWM

  if (qss)
    w->setStyleSheet(SS_WINDOW);

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
}

void
UiStyle::setMenuStyle(QMenu *menu, bool qss)
{
  Q_ASSERT(menu);
  if (!menu)
    return;

#ifdef USE_WIN_DWM
  DWM_ENABLE_AERO_WIDGET(menu);
  if (!Dwm::isCompositionEnabled())
#endif // USE_WIN_DWM
  if (qss)
    menu->setStyleSheet(SS_MENU);
}

void
UiStyle::setContextMenuStyle(QMenu *menu, bool qss)
{
  Q_ASSERT(menu);
  if (!menu)
    return;

#ifdef USE_WIN_DWM
  DWM_ENABLE_ONETIME_AERO_WIDGET(menu);
  if (Dwm::isCompositionEnabled()) {
    if (qss)
      menu->setStyleSheet(SS_CONTEXTMENU_DWM);
  } else
#endif // USE_WIN_DWM
  if (qss)
    menu->setStyleSheet(SS_CONTEXTMENU);

#ifdef Q_WS_WIN
  QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(menu);
  effect->setColor(G_CONTEXTMENU_COLOR);
  effect->setStrength(G_CONTEXTMENU_COLOR_STRENGTH);
  menu->setGraphicsEffect(effect);
#else
  //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(menu);
  //menu->setGraphicsEffect(blur);
  //QGraphicsOpacityEffect *transparent = new QGraphicsOpacityEffect(menu);
  //menu->setGraphicsEffect(transparent);
  //menu->setWindowOpacity(G_CONTEXTMENU_OPACITY);
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
#endif
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
