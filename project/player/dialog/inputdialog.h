#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

// inputdialog.h
// 2/8/2012

#include "module/qtext/dialog.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QComboBox)

class AcComboEdit;

typedef QtExt::Dialog InputDialogBase;
class InputDialog : public InputDialogBase
{
  Q_OBJECT
  typedef InputDialog Self;
  typedef InputDialogBase Base;

  AcComboEdit *edit_;

public:
  explicit InputDialog(QWidget *parent = 0);

signals:
  void textEntered(QString text);

public slots:
  void setText(const QString &t);
  void setToolTip(const QString &t);
  void setDefaultItems(const QStringList &l);

protected slots:
  void ok();

protected:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);

private:
  void createLayout();
};

#endif // INPUTDIALOG_H
