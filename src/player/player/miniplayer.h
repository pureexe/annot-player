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
  explicit MiniPlayerUi(SignalHub *hub, Player *player, AnnotationServerAgent *server, QWidget *parent = nullptr);

public:
  void setTitle(const QString &title) override;

public slots:
  void updateTitle();
  void setVisible(bool visible) override;

  // - Events -
protected:
  void forward(QEvent *event);

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void mouseDoubleClickEvent(QMouseEvent *event) override;

  void contextMenuEvent(QContextMenuEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

protected slots:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

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
  explicit MiniPlayerDock(QWidget *parent = nullptr);

public:
  void setVisible(bool visible) override; // stop polling when hidden

signals:
  void toggleFullScreenModeRequested();
  void toggleMiniModeRequested();

protected:
  void forward(QEvent *event);

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
};
*/

