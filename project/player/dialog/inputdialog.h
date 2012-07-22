#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

// inputdialog.h
// 2/8/2012

#include "project/common/acwindow.h"
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QComboBox)

class AcComboEdit;

class InputDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(InputDialog)
  typedef InputDialog Self;
  typedef AcWindow Base;

  AcComboEdit *edit_;

public:
  explicit InputDialog(QWidget *parent = nullptr);

signals:
  void textEntered(QString text);

public slots:
  void setText(const QString &t);
  void setToolTip(const QString &t);
  void setDefaultItems(const QStringList &l);

protected slots:
  void ok();

protected:
  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  //void contextMenuEvent(QContextMenuEvent *event) override;

private:
  void createLayout();
};

#endif // INPUTDIALOG_H
