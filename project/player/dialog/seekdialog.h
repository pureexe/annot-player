#ifndef SEEKDIALOG_H
#define SEEKDIALOG_H

// seekdialog.h
// 8/16/2011

#include "module/qtext/dialog.h"

QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog SeekDialogBase;
class SeekDialog : public SeekDialogBase
{
  Q_OBJECT
  typedef SeekDialog Self;
  typedef SeekDialogBase Base;

  QComboBox *ssEdit_, *mmEdit_, *hhEdit_;

public:
  explicit SeekDialog(QWidget *parent = 0);

  qint64 time() const;

signals:
  void seekRequested(qint64 msecs);

public slots:
  void setTime(qint64 msecs);

protected slots:
  void ok();

protected:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);
};

#endif // SEEKDIALOG_H
