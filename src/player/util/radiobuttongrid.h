#ifndef RADIOBUTTONGRID_H
#define RADIOBUTTONGRID_H

// radiobuttongrid.h
// 8/18/2012

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QRadioButton)

class RadioButtonGrid : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(RadioButtonGrid)
  typedef RadioButtonGrid Self;
  typedef QObject Base;

  const int capacity_;
  int column_;
  QGridLayout *layout_;
  QRadioButton **buttons_;

  int size_;

public:
  RadioButtonGrid(int capacity, int col, QObject *parent = nullptr);
  ~RadioButtonGrid();

signals:
  void error(const QString &text);
  void currentIndexChanged(int index);

  // - Properties -
public:
  QGridLayout *layout() const { return layout_; }

  int capacity() const { return capacity_; }
  int size() const { return size_; }
  bool isEmpty() const { return !size_; }
  bool isFull() const { return size_ == capacity_; }

  bool isItemEnabled(int index) const;
  QString itemText(int index) const;

  QString currentText() const { return itemText(currentIndex()); }
  int currentIndex() const;
  bool hasSelection() const { return currentIndex() >= 0; }
public slots:
  void setCurrentIndex(int index);

  // - Actions -
public slots:
  void setItemText(int index, const QString &text);
  void setItemEnabled(int index, bool enabled);
  void setItemsEnabled(bool t);
  void addItem(const QString &text = QString());
  void clear();

protected slots:
  void invalidateCurrentIndex()
  { emit currentIndexChanged(currentIndex()); }

private:
  void createLayout();
};

#endif // RadioBUTTONGRID_H
