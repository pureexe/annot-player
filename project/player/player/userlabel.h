#ifndef USERLABEL_H
#define USERLABEL_H

// userlabel.h
// 8/1/2011

#include <QtGui/QLabel>

class UserLabel : public QLabel
{
  Q_OBJECT
  Q_DISABLE_COPY(UserLabel)

  typedef UserLabel Self;
  typedef QLabel Base;

public:
  explicit UserLabel(QWidget *parent = nullptr)
    : Base(parent) { }

signals:
  void showUserPanelRequested();

protected:
  void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // USERLABEL_H
