#ifndef WEBBROWSER_P_H
#define WEBBROWSER_P_H

// webbrowser_p.h
// 1/27/2012

#include <QtWebKit/QWebView>
#include <QtGui/QTabWidget>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

// - Slots -

namespace slot_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabText(QTabWidget *w, int index, QObject *parent = 0)
      : Base(parent), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void trigger(const QString &text)
    {
      if (index_ < w_->count()) {
        w_->setTabText(index_, text);
        w_->setTabToolTip(index_, text);
      }
    }

  protected:
    int textMaxCount() const
    {
      enum { MinCount = 3 + 1 };
      enum { FontWidth = 5 };
      if (!w_->count())
        return 0;
      int ret = QApplication::desktop()->width() / (w_->count() * FontWidth);
      return qMax((int)MinCount, ret);
    }
  };

  class SetTabIcon : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QTabWidget *w_;
    int index_;

  public:
    SetTabIcon(QTabWidget *w, int index, QObject *parent = 0)
      : Base(parent), w_(w), index_(index)
    {  Q_ASSERT(w_); Q_ASSERT(index >= 0); }

  public slots:
    void trigger()
    {
      if (index_ < w_->count()) {
        QWebView *t = qobject_cast<QWebView *>(w_->widget(index_));
        if (t)
          w_->setTabIcon(index_, t->icon());
      }
    }
  };

} // namespace slot_

#endif // WEBBROWSER_P_H

/*
    void setTabText(const QString &text)
    {
      if (index_ < w_->count()) {
        w_->setTabToolTip(index_, text);
        int len = textMaxCount();
        if (len && text.size() < len)
          w_->setTabText(index_, text);
        else {
          Q_ASSERT(len > 3);
          QString t = text.left(len - 3) + "...";
          w_->setTabText(index_, t);
        }
      }
    }

  protected:
    int textMaxCount() const
    {
      enum { MinCount = 3 + 1 };
      enum { FontWidth = 5 };
      if (!w_->count())
        return 0;
      int ret = QApplication::desktop()->width() / (w_->count() * FontWidth);
      return qMax((int)MinCount, ret);
    }
  };

  */
