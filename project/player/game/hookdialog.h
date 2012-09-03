#ifndef HOOKDIALOG_H
#define HOOKDIALOG_H

// hookdialog.h
// 9/2/2012

#include "project/common/acmainwindow.h"

QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class HookDialog : public AcMainWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(HookDialog)
  typedef HookDialog Self;
  typedef AcMainWindow Base;

  QComboBox *edit_;
  QToolButton *openButton_;
              //*saveButton_;
  QString editStyleSheet_;

public:
  explicit HookDialog(QWidget *parent = nullptr);

  QString text() const;
  bool isEmpty() const;

signals:
  void hookEntered(const QString &hook);

public slots:
  void open();
  void paste();
  void setText(const QString &hook);

  void setVisible(bool visible) override;

protected slots:
  void verifyEditText();

protected:
  void refresh();
  void updateEnabled();

private:
  void createLayout();
};

#endif // HOOKDIALOG_H
