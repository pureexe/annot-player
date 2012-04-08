#ifndef EMBEDDEDPLAYER_H
#define EMBEDDEDPLAYER_H

// embeddedplayer.h
// 7/11/2011

#include "playerui.h"

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class EmbeddedCanvas;

class EmbeddedPlayerUi : public PlayerUi
{
  Q_OBJECT
  typedef EmbeddedPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit EmbeddedPlayerUi(EmbeddedCanvas *canvas, SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

signals:
  void fullScreenModeChanged(bool t);
  void canvasEnabledChanged(bool t);
public:
  bool autoHideEnabled() const;

  WId containerWindow() const;
  QWidget *containerWidget() const;

  bool isFullScreenMode() const { return fullScreen_; }

  bool isOnTop() const { return top_; }
  bool isCanvasEnabled() const { return canvasEnabled_; }
public slots:
  void setOnTop(bool t);

protected slots:
  void showWhenEmbedded();
  virtual void invalidateInputCountButton(); ///< \override

public slots:
  void setCanvasEnabled(bool enabled);
  void setAutoHideEnabled(bool enabled = true);
  void autoHide();

  void invalidateGeometry();    ///< Automatically adjust from its parent
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
  EmbeddedCanvas *canvas_;
  QTimer *autoHideTimer_;
  QTimer *trackingTimer_;

  QToolButton *menuButton_;

  WId containerWindow_;
  QWidget *containerWidget_;

  bool fullScreen_,
       top_,
       canvasEnabled_;
};

#endif // EMBEDDEDPLAYER_H
