#ifndef SYNCVIEW_H
#define SYNCVIEW_H

// syncview.h
// 8/13/2011

#include "textthread.h"
#include "processinfo.h"
#include "src/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)

class ProcessView;
class ThreadView;
class MessageHandler;
//class TokenView;

class SyncView : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SyncView)
  typedef SyncView Self;
  typedef AcMainWindow Base;

  ProcessView *processView_;
  ThreadView *threadView_;
  MessageHandler *messageHandler_;
  //TokenView *tokenView_;

  QToolButton *processButton_, *threadButton_;

public:
  explicit SyncView(MessageHandler *h, QWidget *parent = nullptr);

signals:
  void threadsSelected(const TextThreadList &threads, ProcessInfo pi);

public:
  //TokenView *tokenView() const { return tokenView_; }
  ProcessView *processView() const { return processView_; }
  ThreadView *threadView() const { return threadView_; }

public slots:
  void setProcessViewVisible(bool t);
  void setThreadViewVisible(bool t);
  void showThreadView();
  void disableThreadView();

  // - Events -
public slots:
  //void setVisible(bool visible) override;

//protected slots:
//  void dragEnterEvent(QDragEnterEvent *event) override;
//  void dragMoveEvent(QDragMoveEvent *event) override;
//  void dragLeaveEvent(QDragLeaveEvent *event) override;
//  void dropEvent(QDropEvent *event) override;
//
//signals:
//  void dragEnterEventReceived(QDragEnterEvent *event);
//  void dragMoveEventReceived(QDragMoveEvent *event);
//  void dragLeaveEventReceived(QDragLeaveEvent *event);
//  void dropEventReceived(QDropEvent *event);

private slots:
  void select(const TextThreadList &threads);
private:
  void createLayout();
};

#endif // SYNCVIEW_H
