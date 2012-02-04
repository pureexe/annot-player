#ifndef PICKDIALOG_H
#define PICKDIALOG_H

// pickdialog.h
// 10/30/2011

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

typedef QtExt::Dialog PickDialogBase;
class PickDialog : public PickDialogBase
{
  Q_OBJECT
  typedef PickDialog Self;
  typedef PickDialogBase Base;

public:
  explicit PickDialog(QWidget *parent = 0);

signals:
  void windowPicked(WId winId);

  // - Properties -
public:
  void setMessage(const QString &text);

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override
  virtual void keyPressEvent(QKeyEvent *event); ///< \override

  void setActive(bool t);
  bool isActive() const;

  // - Slots -
protected slots:
  void cancel();
  void pickWindow(WId winId);

  void startPicking();
  void stopPicking();

private:
  bool active_;
  QLabel *messageLabel_;
};

#endif // PICKDIALOG_H
