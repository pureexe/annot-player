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

public slots:
  //virtual void mousePressEvent(QMouseEvent *event);
  //virtual void mouseMoveEvent(QMouseEvent *event);
  //virtual void mouseReleaseEvent(QMouseEvent *event);
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);

#ifdef USE_WIN_HOOK
  // Be careful to add child widget to this class!!
public slots:
  void addToWindowsHook();
  void removeFromWindowsHook();
  bool containsWindow(WId winId) const;
private:
  QList<WId> children_, history_;
#endif // USE_WIN_HOOK

#ifdef Q_WS_MAC
private:
  void *view_; // cocoa view
public:
  void *view() const;
  bool isViewVisible() const;
public slots:
  void setViewVisible(bool visible = true);
  void showView();
  void hideView();
#endif // Q_WS_MAC
};

#endif // VIDEOVIEW_H
