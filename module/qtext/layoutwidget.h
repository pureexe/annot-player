#ifndef _QTEXT_LAYOUTWIDGET_H
#define _QTEXT_LAYOUTWIDGET_H

#include <QtGui/QWidget>

#include <QtGui>
class LayoutWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(LayoutWidget)
  typedef LayoutWidget Self;
  typedef QWidget Base;

public:
  explicit LayoutWidget(QLayout *layout, QWidget *parent = 0)
    : Base(parent)
  {
    setLayout(layout);
    setContentsMargins(0, 0, 0, 0);
  }
};

#endif // _QTEXT_LAYOUTWIDGET_H
