#ifndef WEBBROWSERPRIVATE_H
#define WEBBROWSERPRIVATE_H

// WEBBROWSERPRIVATE.h
// 1/27/2012

#include <QtCore>
#include <QtGui>

// - Slots -

namespace slot_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabText(QTabWidget *w, int index)
      : Base(w), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void setTabText(const QString &text)
    {
      if (w_->count() > index_)
        w_->setTabText(index_, text);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace slot_

#endif // WEBBROWSERPRIVATE_H
