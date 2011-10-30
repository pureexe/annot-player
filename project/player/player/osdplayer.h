#ifndef OSDPLAYER_H
#define OSDPLAYER_H

// osdplayer.h
// 7/11/2011

#include "playerui.h"
#include <QObject>

QT_FORWARD_DECLARE_CLASS(QTimer)

class OSDPlayerUi : public PlayerUi
{
  Q_OBJECT
  typedef OSDPlayerUi Self;
  typedef PlayerUi Base;

  QTimer *autoHideTimer_;

public:
  explicit OSDPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

  bool autoHideEnabled() const;
  void setAutoHideEnabled(bool enabled = true);

public slots:
  void autoHide();

  void invalidateGeometry();    ///< Automatically adjust from its parent.
  void resetAutoHideTimeout();  ///< Reset timeout for autohide OSDPlayer

  void resetAutoHideTimeoutWhenEditing(const QString&) { return resetAutoHideTimeout(); }
  void resetAutoHideTimeoutWhenEditing(int, int) { return resetAutoHideTimeout(); }

private:
  void createLayout();
};

#endif // OSDPLAYER_H
