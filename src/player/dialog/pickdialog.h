#ifndef PICKDIALOG_H
#define PICKDIALOG_H

// pickdialog.h
// 10/30/2011

#include "src/common/acwindow.h"

QT_FORWARD_DECLARE_CLASS(QLabel)

class PickDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(PickDialog)
  typedef PickDialog Self;
  typedef AcWindow Base;

public:
  explicit PickDialog(QWidget *parent = nullptr);

signals:
  void message(QString text);
  void windowPicked(WId winId);
  void cancelled();

  // - Properties -
public:
  void setMessage(const QString &text);

  // - Events -
public:
  void setVisible(bool visible) override;
  void keyPressEvent(QKeyEvent *event) override;

  void setActive(bool t);
  bool isActive() const { return active_; }

  // - Slots -
protected slots:
  void cancel();
  void pickWindow(WId winId);

  void startPicking();
  void stopPicking();

private:
  void createLayout();

private:
  bool active_;
  WId pickedWindow_;
  QLabel *messageLabel_;
};

#endif // PICKDIALOG_H
