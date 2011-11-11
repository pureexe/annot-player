#ifndef MAINWINDOWPRIVATE_H
#define MAINWINDOWPRIVATE_H

// mainwindowprivate.h
// 11/8/2011

#include "mainwindow.h"
#ifdef USE_WIN_HOOK
  #include "win/hook/hook.h"
#endif // USE_WIN_HOOK
#include <QtGui>

// - Slots -

namespace MainWindow_slot_ { // anonymous slot_

#ifdef USE_MODE_SIGNAL
  class OpenProcessId : public QObject
  {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    ulong pid_;

  public:
    OpenProcessId(ulong pid, MainWindow *w)
      : Base(w), pid_(pid), w_(w)
    { Q_ASSERT(w_); Q_ASSERT(pid_); }

  public slots:
    void openProcessId()
    {
      w_->openProcessId(pid_);
      w_->openProcess();
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };
#endif // USE_MODE_SIGNAL

} // anonymous namespace slot_

// - Tasks -

namespace { namespace task_ { // anonymous

  class login : public QRunnable
  {
    MainWindow *w_;
    QString name_, pass_;
    virtual void run() { w_->login(name_, pass_, false); } // \override, async = false
  public:
    login(const QString userName, const QString &password, MainWindow *w)
      : w_(w), name_(userName), pass_(password) { Q_ASSERT(w_); }
  };

  class logout : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->logout(false); } // \override, async = false
  public:
    explicit logout(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class setUserAnonymous : public QRunnable
  {
    MainWindow *w_;
    bool t_;
    virtual void run() { w_->setUserAnonymous(t_, false); } // \override, async = false
  public:
    setUserAnonymous(bool t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class setUserLanguage : public QRunnable
  {
    MainWindow *w_;
    int l_;
    virtual void run() { w_->setUserLanguage(l_, false); } // \override, async = false
  public:
    setUserLanguage(int l, MainWindow *w) : w_(w), l_(l) { Q_ASSERT(w_); }
  };

  class chat : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->chat(text_, false); } // \override, async = false
  public:
    chat(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->submitText(text_, false); } // \override, async = false
  public:
    submitText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitAlias : public QRunnable
  {
    MainWindow *w_;
    Alias a_;
    virtual void run() { w_->submitAlias(a_, false); } // \override, async = false
  public:
    submitAlias(const Alias &a, MainWindow *w) : w_(w), a_(a) { Q_ASSERT(w_); }
  };

  class setToken : public QRunnable
  {
    MainWindow *w_;
    QString t_;
    virtual void run() { w_->setToken(t_, false); } // \override, async = false
  public:
    setToken(const QString &t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

} } // anonymous namespace task_

// - Hook -
#ifdef USE_WIN_HOOK

class HookEventForwarder : public QObject
{
  Q_OBJECT
  typedef HookEventForwarder Self;
  typedef QObject Base;

  MainWindow *w_;

public:
  explicit HookEventForwarder(MainWindow *w)
    : Base(w), w_(w) { Q_ASSERT(w_); }

protected:
  virtual bool
  eventFilter(QObject *hook, QEvent *event) // \override  Hook event filter.
  {
    // jichi 8/2/2011: Ugly fix for cancelling context menu.
    static bool contextMenuPoppedUp = false;

    if (event)
      switch (event->type()) {
      case QEvent::ContextMenu:
        contextMenuPoppedUp = true;
        QCoreApplication::sendEvent(w_, event);
        return true;

        // Forward
      case QEvent::MouseButtonDblClick:
        contextMenuPoppedUp = false;
      case QEvent::MouseMove:
      case QEvent::Wheel:
        QCoreApplication::sendEvent(w_, event);
        return true;

      case QEvent::MouseButtonPress: {
          QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
          Q_ASSERT(mouseEvent);
          if (mouseEvent->button() == Qt::LeftButton
              && !contextMenuPoppedUp)
            if (w_->isPlaying() || w_->isEditing())
              w_->pause();
        }
        contextMenuPoppedUp = false;
      case QEvent::MouseButtonRelease:
        event->accept();
        return true;

      //case QEvent::KeyPress:
      //case QEvent::KeyRelease:
      //  //QCoreApplication::sendEvent(w_, event);
      //  if (w_->visiblePlayer())
      //    w_->visiblePlayer()->lineEdit()->clearFocus();
      //  w_->setFocus();
      //  return true;
      }

    return Base::eventFilter(hook, event);
  }
};
#endif // USE_WIN_HOOK

// - Helpers -

namespace { // anonymous

  template <typename L>
  inline L
  revertList(const L &l)
  {
    L ret;
    if (!l.isEmpty())
      foreach (typename L::value_type v, l)
        ret.push_front(v);
    return ret;
  }

} // anonymous namespace

#endif // MAINWINDOWPRIVATE_H
