#ifndef SYNCVIEW_H
#define SYNCVIEW_H

// syncview.h
// 8/13/2011

#include "textthread.h"
#include "processinfo.h"
#include "project/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)

class ProcessView;
class MessageView;
class MessageHandler;
//class TokenView;

class SyncView : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SyncView)
  typedef SyncView Self;
  typedef AcMainWindow Base;

  ProcessView *processView_;
  MessageView *messageView_;
  MessageHandler *messageHandler_;
  //TokenView *tokenView_;

  QToolButton *processButton_, *messageButton_;

public:
  explicit SyncView(MessageHandler *h, QWidget *parent = nullptr);

signals:
  void threadsSelected(const TextThreadList &threads, ProcessInfo pi);

public:
  //TokenView *tokenView() const { return tokenView_; }
  ProcessView *processView() const { return processView_; }
  MessageView *messageView() const { return messageView_; }

public slots:
  void setProcessViewVisible(bool t);
  void setMessageViewVisible(bool t);
  void showMessageView();
  void disableMessageView();

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
