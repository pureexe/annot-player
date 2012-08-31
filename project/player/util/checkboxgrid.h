#ifndef CHECKBOXGRID_H
#define CHECKBOXGRID_H

// checkboxgrid.h
// 8/27/2012

#include <QtCore/QObject>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QGridLayout)

class CheckBoxGrid : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(CheckBoxGrid)
  typedef CheckBoxGrid Self;
  typedef QObject Base;

  const int capacity_;
  int column_;
  QGridLayout *layout_;
  QCheckBox **buttons_;

  int size_;

public:
  CheckBoxGrid(int capacity, int col, QObject *parent = nullptr);
  ~CheckBoxGrid();

signals:
  void error(const QString &text);
  void selectionChanged();

  // - Properties -
public:
  QGridLayout *layout() const { return layout_; }

  int capacity() const { return capacity_; }
  int size() const { return size_; }
  bool isEmpty() const { return !size_; }
  bool isFull() const { return size_ == capacity_; }
  QString itemText(int index) const;
  bool isItemEnabled(int index) const;

  QList<int> currentIndices() const;
  int itemCount() const;
  bool hasSelection() const;
  bool contains(int index) const;
public slots:
  void setCurrentIndices(const QList<int> &indices);
  void setItemEnabled(int index, bool enabled);
  void setItemsEnabled(bool enabled);
  void setItemText(int index, const QString &text);

  // - Actions -
public slots:
  void addItem(const QString &text = QString());
  void setItemChecked(int index, bool t);
  void clear();

private:
  void createLayout();
};

#endif // RadioBUTTONGRID_H
