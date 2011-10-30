#ifndef SIGNALVIEW_H
#define SIGNALVIEW_H

// signalview.h
// 8/13/2011

#include "processinfo.h"
#include "core/gui/dialog.h"

class ProcessView;
class MessageView;
class TokenView;

typedef Core::Gui::Dialog SignalViewBase;

class SignalView : public SignalViewBase
{
  Q_OBJECT
  typedef SignalView Self;
  typedef SignalViewBase Base;

  ProcessView *processView_;
  MessageView *messageView_;
  TokenView *tokenView_;

public:
  explicit SignalView(QWidget *parent = 0);

  TokenView *tokenView() const;
  ProcessView *processView() const;
  MessageView *messageView() const;

signals:
  void hookSelected(int hookId, ProcessInfo pi);

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

private slots:
  void selectHookAndHide(int hookId);
};

#endif // SIGNALVIEW_H
