#ifndef LIVEDIALOG_H
#define LIVEDIALOG_H

// livedialog.h
// 10/7/2011

#include "project/common/acwindow.h"
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QComboBox)

class LiveDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(LiveDialog)
  typedef LiveDialog Self;
  typedef AcWindow Base;

  QComboBox *comboBox_;

  QList<qint64> timeSlots_;
  QList<int> activeCounts_; // number of people watching this media

  int timeSlotIndex_;

public:
  explicit LiveDialog(QWidget *parent = nullptr);

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
  void updateComboBox();

protected:
  //void mouseDoubleClickEvent(QMouseEvent *event) override;
  //void contextMenuEvent(QContextMenuEvent *event) override;

private:
  void createLayout();
};

#endif // LIVEDIALOG_H
