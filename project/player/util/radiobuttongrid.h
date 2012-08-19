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
  void valueChanged(int value);

  // - Properties -
public:
  QGridLayout *layout() const { return layout_; }

  int capacity() const { return capacity_; }
  int size() const { return size_; }
  bool isEmpty() const { return !size_; }
  bool isFull() const { return size_ == capacity_; }

  int value() const; ///< [-1, capacity-1]
public slots:
  void setValue(int value);

  // - Actions -
public slots:
  void addItem(const QString &title = QString(), const QString &tip = QString());
  void clear();

protected slots:
  void updateValue();

private:
  void createLayout();
};

#endif // RadioBUTTONGRID_H
