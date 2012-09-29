#pragma once

// dwmnotifier_p.h
// 7/10/2011
//
// See: http://labs.qt.nokia.com/2009/09/15/using-blur-behind-on-windows/
// Source:
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.h
// http://sic.no-ammo.org/git/index.cgi/Client.git/blob_plain/HEAD:/modules/qtwin/qtwin.cpp

#include "dwm/dwm.h"
#include "dwm/dwmapi_p.h"
#include <QtCore/QSet>
#include <QtGui/QWindow>
#ifdef QT_WIDGETS_LIB
# include <QtWidgets/QWidget>
#endif // QT_WIDGETS_LIB

// - WindowNotifier -

// Invisible background window.
// Inherit QWidget to access protected winEvent.
class DwmWindowNotifier : public QWindow
{
  //Q_OBJECT // Q_OBJECT is not required
  Q_DISABLE_COPY(DwmWindowNotifier)
  typedef DwmWindowNotifier Self;
  typedef QWindow Base;
  QSet<QWindow *> windows_;
#ifdef QT_WIDGETS_LIB
  QSet<QWidget *> widgets_;
#endif // QT_WIDGETS_LIB
public:
  DwmWindowNotifier() { winId(); } // enforce a valid hwnd

  void addWindow(QWindow *w) { windows_.insert(w); }
  void removeWindow(QWindow *w) { windows_.remove(w); }

#ifdef QT_WIDGETS_LIB
  void addWidget(QWidget *w) { widgets_.insert(w); }
  void removeWidget(QWidget *w) { widgets_.remove(w); }
#endif // QT_WIDGETS_LIB

protected:
  bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
};

bool
DwmWindowNotifier::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
  Q_ASSERT(message);
  if (!widgets_.empty() &&
      static_cast<MSG *>(message)->message == WM_DWMCOMPOSITIONCHANGED) {
    bool enabled = Dwm::isCompositionEnabled();
    foreach (QWindow *w, windows_) {
      Q_ASSERT(w);
      //w->setAttribute(Qt::WA_NoSystemBackground, enabled);
      if (enabled) {
        Dwm::enableBlurBehindWindow(w->winId(), true);
        Dwm::extendFrameIntoClientArea(w->winId(), -1, -1, -1, -1);
      } else {
        Dwm::enableBlurBehindWindow(w->winId(), false);
        Dwm::extendFrameIntoClientArea(w->winId(), 0, 0, 0, 0);
      }
      //w->update();
    }
#ifdef QT_WIDGETS_LIB
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
#endif // QT_WIDGETS_LIB
  }

  return Base::nativeEvent(eventType, message, result);
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
