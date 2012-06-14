#ifndef MINIPLAYER_H
#define MINIPLAYER_H

// miniplayer.h
// 7/11/2011

#include "playerui.h"
#include <QtGui/QDockWidget>

class MiniPlayerUi : public PlayerUi
{
  Q_OBJECT
  Q_DISABLE_COPY(MiniPlayerUi)
  typedef MiniPlayerUi Self;
  typedef PlayerUi Base;

  QPoint dragPos_;

public:
  explicit MiniPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

public:
  virtual void setTitle(const QString &title); ///< \reimp

public slots:
  void updateTitle();
  virtual void setVisible(bool visible); ///< \reimp

  // - Events -
protected:
  void forward(QEvent *event);

  virtual void mousePressEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \reimp
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \reimp

  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \reimp

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \reimp

  virtual void keyPressEvent(QKeyEvent *event); ///< \reimp
  virtual void keyReleaseEvent(QKeyEvent *event); ///< \reimp

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \reimp
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \reimp
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \reimp
  virtual void dropEvent(QDropEvent *event); ///< \reimp

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

private:
  void createLayout();
};

#endif // MINIPLAYER_H

/*
class MiniPlayerDock : public QDockWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(MiniPlayerDock)
  typedef MiniPlayerDock Self;
  typedef QDockWidget Base;

  QPoint dragPos_;

public:
  explicit MiniPlayerDock(QWidget *parent = 0);

public:
  virtual void setVisible(bool visible); // stop polling when hidden

signals:
  void toggleFullScreenModeRequested();
  void toggleMiniModeRequested();

protected:
  void forward(QEvent *event);

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void closeEvent(QCloseEvent *event);
  virtual void contextMenuEvent(QContextMenuEvent *event);

  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dragLeaveEvent(QDragLeaveEvent *event);
  virtual void dropEvent(QDropEvent *event);

  virtual void keyPressEvent(QKeyEvent *event);
  virtual void keyReleaseEvent(QKeyEvent *event);
};
*/

