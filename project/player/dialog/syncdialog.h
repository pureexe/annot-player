#ifndef SYNCDIALOG_H
#define SYNCDIALOG_H

// syncdialog.h
// 10/7/2011

#include "module/qtext/dialog.h"
#include <QList>

QT_FORWARD_DECLARE_CLASS(QComboBox)

typedef QtExt::Dialog SyncDialogBase;
class SyncDialog : public SyncDialogBase
{
  Q_OBJECT
  typedef SyncDialog Self;
  typedef SyncDialogBase Base;

  QComboBox *comboBox_;

  QList<qint64> timeSlots_;
  QList<int> activeCounts_; // number of people watching this media

  int timeSlotIndex_;

public:
  explicit SyncDialog(QWidget *parent = 0);

signals:
  void timeSlotSelected(qint64 secs);

public:
  const QList<qint64> &timeSlots() const;
  const QList<int> &activeCounts() const;
  int timeSlotIndex() const;

public slots:
  void setTimeSlots(const QList<qint64> &timeSlots);
  void setActiveCounts(const QList<int> &counts);
  void setTimeSlotIndex(int index);

  virtual void setVisible(bool visible); ///< \override

protected slots:
  void ok();
  void cancel();
  void updateComboBox();

protected:
  //virtual void mouseDoubleClickEvent(QMouseEvent *event);
  //virtual void contextMenuEvent(QContextMenuEvent *event);
};

#endif // SYNCDIALOG_H
