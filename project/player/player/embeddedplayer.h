#ifndef EMBEDDEDPLAYER_H
#define EMBEDDEDPLAYER_H

// embeddedplayer.h
// 7/11/2011

#include "playerui.h"

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class EmbeddedPlayerUi : public PlayerUi
{
  Q_OBJECT
  typedef EmbeddedPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit EmbeddedPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

signals:
  void invalidateMenuRequested();
  void fullScreenModeChanged(bool t);
public:
  bool autoHideEnabled() const;

  WId containerWindow() const;
  QWidget *containerWidget() const;

  void setMenu(QMenu *menu);
  bool isFullScreenMode() const;

  bool isOnTop() const;
public slots:
  void setOnTop(bool t);

protected:
  QToolButton *menuButton();
protected slots:
  void popupMenu();
  void showWhenEmbedded();

public slots:
  void setAutoHideEnabled(bool enabled = true);
  void autoHide();

  void invalidateGeometry();    ///< Automatically adjust from its parent.
  void resetAutoHideTimeout();  ///< Reset timeout for autohide EmbeddedPlayer

  void setContainerWindow(WId winId);
  void setContainerWidget(QWidget *w);

  virtual void setVisible(bool visible); ///< \override

  void setFullScreenMode(bool t = true);

protected:
  void moveToGlobalPos(const QPoint &globalPos);

protected slots:
  void startTracking();
  void stopTracking();
  void invalidateTrackingTimer();

private:
  void createLayout();

private:
  QTimer *autoHideTimer_;
  QTimer *trackingTimer_;

  QToolButton *menuButton_;

  WId containerWindow_;
  QWidget *containerWidget_;

  bool fullScreen_;
  bool top_;
};

#endif // EMBEDDEDPLAYER_H
