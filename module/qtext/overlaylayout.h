#ifndef _QTEXT_OVERLAYLAYOUT_H
#define _QTEXT_OVERLAYLAYOUT_H

#include <QtGui/QGridLayout>

class OverlayLayout : public QGridLayout
{
  Q_OBJECT
  Q_DISABLE_COPY(OverlayLayout)
  typedef OverlayLayout Self;
  typedef QGridLayout Base;

public:
  explicit OverlayLayout(QWidget *parent = 0)
    : Base(parent) { }

  void addWidget(QWidget *w, Qt::Alignment alignment = 0)
  { Base::addWidget(w, 0, 0, alignment); }

  void addLayout(QLayout *l, Qt::Alignment alignment = 0)
  { Base::addLayout(l, 0, 0, alignment); }
};

#endif // _QTEXT_OVERLAYLAYOUT_H
