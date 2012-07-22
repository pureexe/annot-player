#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

// videoview.h
// 7/10/2011
#include <QtGlobal>
#ifdef Q_OS_MAC
# include <QtGui/QMacCocoaViewContainer>
  typedef QMacCocoaViewContainer VideoViewBase;
  struct vlcvideoview_t;
#elif defined(Q_OS_LINUX)
# include <QtGui/QX11EmbedContainer>
  typedef QX11EmbedContainer VideoViewBase;
#else
# include <QtGui/QFrame>
  typedef QFrame VideoViewBase;
#endif // Q_OS_LINUX
#include <QtCore/QList>

// class VideoView
///  Media player screen container (parent).
class VideoView : public VideoViewBase
{
  Q_OBJECT
  Q_DISABLE_COPY(VideoView)
  typedef VideoView Self;
  typedef VideoViewBase Base;

public:
  explicit VideoView(QWidget *parent = nullptr);
  ~VideoView();

protected:
  //bool macEvent(EventHandlerCallRef caller, EventRef event) override;

#ifdef Q_OS_LINUX
  bool x11Event(XEvent *event) override;
#endif // Q_OS_LINUX

#ifdef WITH_WIN_HOOK
  // Be careful to add child widget to this class!!
public slots:
  void addToWindowsHook();
  void removeFromWindowsHook();
  bool containsWindow(WId winId) const;
private:
  QList<WId> children_;
#endif // WITH_WIN_HOOK

#ifdef Q_OS_MAC
private:
  vlcvideoview_t *view_; // cocoa view
  qint64 pressTime_, releaseTime_;
public:
  vlcvideoview_t *view() const { return view_; }
  bool isViewVisible() const;

  void setViewMousePressPos(const QPoint &globalPos);
  void setViewMouseReleasePos(const QPoint &globalPos);
  void setViewMouseMovePos(const QPoint &globalPos);
  QPoint viewMapFromGlobal(const QPoint &globalPos);

public slots:
  void setViewVisible(bool visible = true);
  void showView();
  void hideView();
#endif // Q_OS_MAC
};

#endif // VIDEOVIEW_H
