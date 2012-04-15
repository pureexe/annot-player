#ifndef WEBBROWSER_P_H
#define WEBBROWSER_P_H

// webbrowser_p.h
// 1/27/2012

#include <QTabWidget>

// - Slots -

namespace slot_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabText(QTabWidget *w, int index, QWidget *parent = 0)
      : Base(parent), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void setTabText(const QString &text)
    {
      if (index_ < w_->count())
        w_->setTabText(index_, text);
    }
  };

} // namespace slot_

#endif // WEBBROWSER_P_H
