#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

// videoview.h
// 7/10/2011
#include <QtGlobal>

#ifdef Q_WS_MAC
  #include <QMacCocoaViewContainer>
  typedef QMacCocoaViewContainer VideoViewBase;
#elif defined(Q_WS_X11)
  #include <QX11EmbedContainer>
  typedef QX11EmbedContainer VideoViewBase;
#else
  #include <QFrame>
  typedef QFrame VideoViewBase;
#endif // Q_WS_X11

#include <QList>

// class VideoView
///  Media player screen container (parent).
class VideoView : public VideoViewBase
{
  Q_OBJECT
  typedef VideoView Self;
  typedef VideoViewBase Base;

public:
  explicit VideoView(QWidget *parent = 0);
  ~VideoView();

protected:
  //virtual bool macEvent(EventHandlerCallRef caller, EventRef event); ///< \override

#ifdef Q_WS_X11
  virtual bool x11Event(XEvent *event); ///< \override
#endif // Q_WS_X11

#ifdef USE_WIN_HOOK
  // Be careful to add child widget to this class!!
public slots:
  void addToWindowsHook();
  void removeFromWindowsHook();
  bool containsWindow(WId winId) const;
private:
  QList<WId> children_;
#endif // USE_WIN_HOOK

#ifdef Q_WS_MAC
private:
  void *view_; // cocoa view
public:
  void *view() const;
  bool isViewVisible() const;

  void setViewMousePressPos(const QPoint &globalPos);
  void setViewMouseReleasePos(const QPoint &globalPos);
  void setViewMouseMovePos(const QPoint &globalPos);
  QPoint viewMapFromGlobal(const QPoint &globalPos);

public slots:
  void setViewVisible(bool visible = true);
  void showView();
  void hideView();
#endif // Q_WS_MAC
};

#endif // VIDEOVIEW_H
