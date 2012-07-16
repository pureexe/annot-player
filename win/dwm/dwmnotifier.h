#pragma once

// dwmnotifier.h
// 7/10/2011
//
// See: http://labs.qt.nokia.com/2009/09/15/using-blur-behind-on-windows/
// Source:
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.h
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.cpp

#include "dwm/dwm.h"
#include "dwm/dwmapi.h"

// - WindowNotifier -

// Invisible background window.
// Inherit QWidget to access protected \fn winEvent.
class DwmWindowNotifier : public QWidget
{
  //Q_OBJECT // Q_OBJECT is not required
  Q_DISABLE_COPY(DwmWindowNotifier)
  typedef DwmWindowNotifier Self;
  typedef QWidget Base;
  QWidgetList widgets_;
public:
  DwmWindowNotifier() { winId(); } // enforce a valid hwnd

  void addWidget(QWidget *w) { if (widgets_.indexOf(w) < 0) widgets_.append(w); }
  void removeWidget(QWidget *w) { widgets_.removeAll(w); }

protected:
  virtual bool winEvent(MSG *message, long *result); ///> override
};

bool
DwmWindowNotifier::winEvent(MSG *message, long *result)
{
  Q_ASSERT(message);
  if (!widgets_.empty() &&
      message && message->message == WM_DWMCOMPOSITIONCHANGED) {
    bool enabled = Dwm::isCompositionEnabled();
    foreach (QWidget *w, widgets_) {
      Q_ASSERT(w);
      w->setAttribute(Qt::WA_NoSystemBackground, enabled);
      if (enabled) {
        Dwm::enableBlurBehindWindow(w->winId(), true);
        Dwm::extendFrameIntoClientArea(w->winId(), -1, -1, -1, -1);
      } else {
        Dwm::enableBlurBehindWindow(w->winId(), false);
        Dwm::extendFrameIntoClientArea(w->winId(), 0, 0, 0, 0);
      }
      w->update();
    }
  }

  return Base::winEvent(message, result);
}

#ifdef WITH_DWM_NOTIFIER
namespace detail { // static
  inline DwmWindowNotifier *windowNotifier() { static DwmWindowNotifier g; return &g; }
} // namespace detail
# define DWM_NOTIFIER       detail::windowNotifier()
#else
# define DWM_NOTIFIER       Q_ASSERT(0)
#endif // WITH_DWM_NOTIFIER

// EOF
