#ifndef SYNCDIALOG_H
#define SYNCDIALOG_H

// syncdialog.h
// 10/7/2011

#include "src/common/acwindow.h"
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QComboBox)

class SyncDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(SyncDialog)
  typedef SyncDialog Self;
  typedef AcWindow Base;

  QComboBox *comboBox_;

  QList<qint64> timeSlots_;
  QList<int> activeCounts_; // number of people watching this media

  int timeSlotIndex_;

public:
  explicit SyncDialog(QWidget *parent = nullptr);

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

  void setVisible(bool visible) override;

protected slots:
  void ok();
  void cancel();
  void updateComboBox();

private:
  void createLayout();
};

#endif // SYNCDIALOG_H
