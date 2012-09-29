// skui.cc
// 9/22/2012

#include "sakurakit/skui.h"
#include "qx/qxtoolbutton.h"
#ifdef WITH_WIN_DWM
# include "dwm/dwm.h"
#endif // WITH_WIN_DWM
#include <QtGui>

SK_BEGIN_NAMESPACE

void
SkUi::setAeroEffect(QWindow *w, bool enable)
{
#ifdef WITH_WIN_DWM
  if (w && Dwm::isCompositionEnabled()) {
    if (enable) {
      //Dwm::enableBlurBehindWindow(w);
      Dwm::extendFrameIntoClientArea(w);
    } else {
      //Dwm::enableBlurBehindWindow(w, false);
      Dwm::extendFrameIntoClientArea(w, 0, 0, 0, 0);
    }
  }
#endif // WITH_WIN_DWM
}

void
SkUi::setAeroEffect(QWidget *w, bool enable)
{
#ifdef WITH_WIN_DWM
  if (w && Dwm::isCompositionEnabled()) {
    if (enable) {
      //Dwm::enableBlurBehindWindow(w);
      Dwm::extendFrameIntoClientArea(w);

      w->setAttribute(Qt::WA_TranslucentBackground);
      //w->setAttribute(Qt::WA_NoSystemBackground);
    } else {
      //Dwm::enableBlurBehindWindow(w, false);
      Dwm::extendFrameIntoClientArea(w, 0, 0, 0, 0);
    }
  }
#endif // WITH_WIN_DWM
}

  /*
QToolButton*
SkUi::createToolButton(ulong hints, const QString &title, const QString &tip, const QString &key,
                        QObject *receiver, const char *slot, Qt::ConnectionType type)
{
  QToolButton *ret = new QxToolButton;
  ret->setToolButtonStyle(Qt::ToolButtonTextOnly);

  ret->setStyleSheet(
        hints & InvertHint ? ACSS_TOOLBUTTON_TEXT_INVERT :
        hints & HighlightHint ? ACSS_TOOLBUTTON_TEXT_HIGHLIGHT :
        hints & PushHint ? ACSS_TOOLBUTTON_TEXT_NORMAL :
        hints & CheckHint ? ACSS_TOOLBUTTON_TEXT_CHECKABLE :
        hints & TabHint ? ACSS_TOOLBUTTON_TEXT_TAB :
        hints & UrlHint ? ACSS_TOOLBUTTON_TEXT_URL :
        ACSS_TOOLBUTTON_TEXT
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

  ret->setGraphicsEffect(makeHaloEffect(hints));

  if (receiver && slot) {
    if (hints & (CheckHint | TabHint | UrlHint))
      connect(ret, SIGNAL(clicked(bool)), receiver, slot, type);
    else
      connect(ret, SIGNAL(clicked()), receiver, slot, type);
  } else if (hints & MenuHint)
    connect(ret, SIGNAL(clicked()), ret, SLOT(showMenu()));

  return ret;
}
  */

SK_END_NAMESPACE

