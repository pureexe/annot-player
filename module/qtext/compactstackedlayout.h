#ifndef _QTEXT_COMPACTSTACKEDLAYOUT_H
#define _QTEXT_COMPACTSTACKEDLAYOUT_H

// compactstackedlayout.h
// 5/9/2012

#include <QtGui/QStackedLayout>
#include <QtGui/QWidget>

class CompactStackedLayout : public QStackedLayout
{
  Q_OBJECT
  Q_DISABLE_COPY(CompactStackedLayout)
  typedef CompactStackedLayout Self;
  typedef QStackedLayout Base;

public:
  CompactStackedLayout() { }
  explicit CompactStackedLayout(QWidget *parent) : Base(parent) { }
  explicit CompactStackedLayout(QLayout *parentLayout) : Base(parentLayout) { }

  virtual QSize sizeHint() const ///< \reimp
  {
    QWidget *w = currentWidget();
    return w ? w->sizeHint() : QSize();
  }
};

#endif // _QTEXT_COMPACTSTACKLAYOUT_H
