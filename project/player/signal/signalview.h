#ifndef SIGNALVIEW_H
#define SIGNALVIEW_H

// signalview.h
// 8/13/2011

#include "processinfo.h"
#include "project/common/acmainwindow.h"

class ProcessView;
class MessageView;
//class TokenView;

class SignalView : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SignalView)
  typedef SignalView Self;
  typedef AcMainWindow Base;

  ProcessView *processView_;
  MessageView *messageView_;
  //TokenView *tokenView_;

public:
  explicit SignalView(QWidget *parent = nullptr);

  //TokenView *tokenView() const { return tokenView_; }
  ProcessView *processView() const { return processView_; }
  MessageView *messageView() const { return messageView_; }

signals:
  void hookSelected(ulong hookId, ProcessInfo pi);

  // - Events -
public:
  void setVisible(bool visible) override;

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
  void selectHookAndHide(ulong hookId);
private:
  void createLayout();
};

#endif // SIGNALVIEW_H
