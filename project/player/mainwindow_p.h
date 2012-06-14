#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

// mainwindow_p.h
// 11/8/2011

#include "mainwindow.h"
#ifdef WITH_WIN_HOOK
#  include "win/hook/hook.h"
#endif // WITH_WIN_HOOK
#include <QtGui>

// - Slots -

namespace slot_ { // anonymous slot_

  class OpenSource : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QString url_;

  public:
    OpenSource(const QString &url, MainWindow *w)
      : Base(w), w_(w), url_(url)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->openSource(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

  class ImportAnnotationsFromUrl : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QString url_;

  public:
    ImportAnnotationsFromUrl(const QString &url, MainWindow *w)
      : Base(w), w_(w), url_(url)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->importAnnotationsFromUrl(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

  /*
  class CloseEventLeave : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QCloseEvent *e_;

  public:
    CloseEventLeave(QCloseEvent *e, MainWindow *w)
      : Base(0), w_(w), e_(e) // no parent
    { Q_ASSERT(w_); Q_ASSERT(e_); }

  public slots:
    void trigger()
    {
      w_->closeEventLeave(e_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

  class SetWindowDwmEnabled : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    bool t_;

  public:
    SetWindowDwmEnabled(bool t, MainWindow *w)
      : Base(w), w_(w), t_(t)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->setWindowDwmEnabled(t_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };
  */

#ifdef USE_MODE_SIGNAL
  class OpenProcessId : public QObject
  {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    ulong pid_;

  public:
    OpenProcessId(ulong pid, MainWindow *w)
      : Base(w), w_(w), pid_(pid)
    { Q_ASSERT(w_); Q_ASSERT(pid_); }

  public slots:
    void trigger()
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

  class updateAnnotations : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->updateAnnotations(false); } // \reimp, async = false
  public:
    explicit updateAnnotations(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class invalidateMediaAndPlay : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->invalidateMediaAndPlay(false); } // \reimp, async = false
  public:
    explicit invalidateMediaAndPlay(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class checkInternetConnection : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->checkInternetConnection(false); } // \reimp, async = false
  public:
    explicit checkInternetConnection(MainWindow *w)
      : w_(w) { Q_ASSERT(w_); }
  };

  class login : public QRunnable
  {
    MainWindow *w_;
    QString name_, pass_;
    virtual void run() { w_->login(name_, pass_, false); } // \reimp, async = false
  public:
    login(const QString userName, const QString &password, MainWindow *w)
      : w_(w), name_(userName), pass_(password) { Q_ASSERT(w_); }
  };

  class logout : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->logout(false); } // \reimp, async = false
  public:
    explicit logout(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class setUserAnonymous : public QRunnable
  {
    MainWindow *w_;
    bool t_;
    virtual void run() { w_->setUserAnonymous(t_, false); } // \reimp, async = false
  public:
    setUserAnonymous(bool t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class setUserLanguage : public QRunnable
  {
    MainWindow *w_;
    int l_;
    virtual void run() { w_->setUserLanguage(l_, false); } // \reimp, async = false
  public:
    setUserLanguage(int l, MainWindow *w) : w_(w), l_(l) { Q_ASSERT(w_); }
  };

  class chat : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->chat(text_, false); } // \reimp, async = false
  public:
    chat(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class updateLiveAnnotations : public QRunnable
  {
    MainWindow *w_;
    virtual void run() { w_->updateLiveAnnotations(false); } // \reimp, async = false
  public:
    explicit updateLiveAnnotations(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class submitLiveText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->submitLiveText(text_, false); } // \reimp, async = false
  public:
    submitLiveText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    virtual void run() { w_->submitText(text_, false); } // \reimp, async = false
  public:
    submitText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class submitAlias : public QRunnable
  {
    MainWindow *w_;
    Alias a_;
    virtual void run() { w_->submitAlias(a_, false); } // \reimp, async = false
  public:
    submitAlias(const Alias &a, MainWindow *w) : w_(w), a_(a) { Q_ASSERT(w_); }
  };

  class setToken : public QRunnable
  {
    MainWindow *w_;
    QString t_;
    virtual void run() { w_->setToken(t_, false); } // \reimp, async = false
  public:
    setToken(const QString &t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class signFileWithUrl : public QRunnable
  {
    MainWindow *w_;
    QString path_;
    QString url_;
    virtual void run() { w_->signFileWithUrl(path_, url_, false); } // \reimp, async = false
  public:
    signFileWithUrl(const QString &path, const QString &url, MainWindow *w)
      : w_(w), path_(path), url_(url) { Q_ASSERT(w_); }
  };


#define CAST(_cast, _entity) \
  class _cast##_entity##WithId : public QRunnable \
  { \
    MainWindow *w_; \
    qint64 id_; \
    virtual void run() { w_->_cast##_entity##WithId(id_, false); } \
  public: \
    _cast##_entity##WithId(qint64 id, MainWindow *w) : w_(w), id_(id) { Q_ASSERT(w_); } \
  };

  CAST(bless, Token) CAST(curse, Token)
  CAST(bless, User) CAST(block, User) CAST(curse, User)
  CAST(bless, Alias) CAST(block, Alias) CAST(curse, Alias)
  CAST(bless, Annotation) CAST(block, Annotation) CAST(curse, Annotation)
#undef CAST

} } // anonymous namespace task_

// - Hook -
#ifdef WITH_WIN_HOOK

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
  eventFilter(QObject *hook, QEvent *event) // \reimp  Hook event filter.
  {
    // jichi 8/2/2011: Ugly fix for cancelling context menu.
    //static bool contextMenuPoppedUp = false;

    if (event)
      switch (event->type()) {
      case QEvent::ContextMenu:
        //contextMenuPoppedUp = true;
        QCoreApplication::sendEvent(w_, event);
        return true;

        // Forward
      case QEvent::MouseButtonDblClick:
        //contextMenuPoppedUp = false;
      case QEvent::MouseMove:
      case QEvent::Wheel:
        QCoreApplication::sendEvent(w_, event);
        return true;

      case QEvent::MouseButtonPress: {
          //QMouseEvent *mouseEvent = qobject_cast<QMouseEvent*>(event);
          //Q_ASSERT(mouseEvent);
          //if (mouseEvent->button() == Qt::LeftButton
          //    && !contextMenuPoppedUp)
          //  if (w_->isPlaying() || w_->isEditing())
          //    w_->pause();
        }
        //contextMenuPoppedUp = false;
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
#endif // WITH_WIN_HOOK


#endif // MAINWINDOW_P_H
