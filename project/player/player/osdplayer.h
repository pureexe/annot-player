#ifndef OSDPLAYER_H
#define OSDPLAYER_H

// osdplayer.h
// 7/11/2011

#include "playerui.h"

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class OsdPlayerUi : public PlayerUi
{
  Q_OBJECT
  typedef OsdPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit OsdPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

signals:
  void invalidateMenuRequested();
public:
  bool autoHideEnabled() const;
  WId trackingWindow() const;

  void setMenu(QMenu *menu);

protected:
  QToolButton *menuButton();
protected slots:
  void popupMenu();

public slots:
  void setAutoHideEnabled(bool enabled = true);
  void autoHide();

  void invalidateGeometry();    ///< Automatically adjust from its parent.
  void resetAutoHideTimeout();  ///< Reset timeout for autohide OsdPlayer

  void resetAutoHideTimeoutWhenEditing(const QString&) { return resetAutoHideTimeout(); }
  void resetAutoHideTimeoutWhenEditing(int, int) { return resetAutoHideTimeout(); }

  void setTrackingWindow(WId winId);

  virtual void setVisible(bool visible); ///< \override

protected:
  void moveToGlobalPos(const QPoint &globalPos);

protected slots:
  void startTracking();
  void stopTracking();

private:
  void createLayout();

private:
  QTimer *autoHideTimer_;
  QTimer *trackingTimer_;

  QToolButton *menuButton_;

  WId trackingWindow_;
};

#endif // OSDPLAYER_H
