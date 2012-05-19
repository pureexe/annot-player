#ifndef EMBEDDEDPLAYER_H
#define EMBEDDEDPLAYER_H

// embeddedplayer.h
// 7/11/2011

#include "playerui.h"

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class DataManager;
class AnnotationGraphicsView;
class EmbeddedCanvas;
class EmbeddedInfoView;

class EmbeddedPlayerUi : public PlayerUi
{
  Q_OBJECT
  Q_DISABLE_COPY(EmbeddedPlayerUi)
  typedef EmbeddedPlayerUi Self;
  typedef PlayerUi Base;

public:
  explicit EmbeddedPlayerUi(SignalHub *hub, Player *player, ServerAgent *server,
                            DataManager *data, AnnotationGraphicsView *annot, QWidget *parent = 0);

signals:
  void fullScreenModeChanged(bool t);
  void visibleChanged(bool t);
public:
  EmbeddedCanvas *canvas() const { return canvas_; }
  EmbeddedInfoView *infoView() const { return infoView_; }

  bool autoHideEnabled() const;

  WId containerWindow() const;
  QWidget *containerWidget() const;

  bool isFullScreenMode() const { return fullScreen_; }

  bool isOnTop() const { return top_; }
public slots:
  void setOnTop(bool t);

protected slots:
  void showWhenEmbedded();
  virtual void updateInputCountButton(); ///< \override

public slots:
  void setAutoHideEnabled(bool enabled = true);
  void autoHide();

  void updateGeometry();    ///< Automatically adjust from its parent
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
  void updateTrackingTimer();

private:
  void createLayout();

private:
  EmbeddedCanvas *canvas_;
  EmbeddedInfoView *infoView_;
  QTimer *autoHideTimer_;
  QTimer *trackingTimer_;

  QToolButton *menuButton_;

  WId containerWindow_;
  QWidget *containerWidget_;

  bool fullScreen_,
       top_;
};

#endif // EMBEDDEDPLAYER_H
