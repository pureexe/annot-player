#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

// mainwindow_p.h
// 11/8/2011

#include "mainwindow.h"
#ifdef WITH_WIN_HOOK
# include "win/hook/hook.h"
#endif // WITH_WIN_HOOK
#include <QtGui>

// - Slots -

namespace detail {

  // - Slots -

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
    QString suburl_;
    QString refurl_;

  public:
    ImportAnnotationsFromUrl(const QString &suburl, const QString &refurl, MainWindow *w)
      : Base(w), w_(w), suburl_(suburl), refurl_(refurl)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->importAnnotationsFromUrl(suburl_, refurl_);
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
      : Base(nullptr), w_(w), e_(e) // no parent
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


  // - Tasks -

  class UpdateOnlineAnnotations : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->updateOnlineAnnotations(false); } // async = false
  public:
    explicit UpdateOnlineAnnotations(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class UpdateOfflineAnnotations : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->updateOfflineAnnotations(false); } // async = false
  public:
    explicit UpdateOfflineAnnotations(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class InvalidateMediaAndPlay : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->invalidateMediaAndPlay(false); } // async = false
  public:
    explicit InvalidateMediaAndPlay(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class CheckInternetConnection : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->checkInternetConnection(false); } // async = false
  public:
    explicit CheckInternetConnection(MainWindow *w)
      : w_(w) { Q_ASSERT(w_); }
  };

  class Login : public QRunnable
  {
    MainWindow *w_;
    QString name_, pass_;
    void run() override { w_->login(name_, pass_, false); } // async = false
  public:
    Login(const QString userName, const QString &password, MainWindow *w)
      : w_(w), name_(userName), pass_(password) { Q_ASSERT(w_); }
  };

  class Logout : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->logout(false); } // async = false
  public:
    explicit Logout(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class SetUserAnonymous : public QRunnable
  {
    MainWindow *w_;
    bool t_;
    void run() override { w_->setUserAnonymous(t_, false); } // async = false
  public:
    SetUserAnonymous(bool t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class SetUserLanguage : public QRunnable
  {
    MainWindow *w_;
    int l_;
    void run() override { w_->setUserLanguage(l_, false); } // async = false
  public:
    SetUserLanguage(int l, MainWindow *w) : w_(w), l_(l) { Q_ASSERT(w_); }
  };

  class Chat : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    void run() override { w_->chat(text_, false); } // async = false
  public:
    Chat(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class UpdateLiveAnnotations : public QRunnable
  {
    MainWindow *w_;
    void run() override { w_->updateLiveAnnotations(false); } // async = false
  public:
    explicit UpdateLiveAnnotations(MainWindow *w) : w_(w) { Q_ASSERT(w_); }
  };

  class SubmitLiveText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    void run() override { w_->submitLiveText(text_, false); } // async = false
  public:
    SubmitLiveText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class SubmitText : public QRunnable
  {
    MainWindow *w_;
    QString text_;
    void run() override { w_->submitText(text_, false); } // async = false
  public:
    SubmitText(const QString &text, MainWindow *w) : w_(w), text_(text) { Q_ASSERT(w_); }
  };

  class SubmitAlias : public QRunnable
  {
    MainWindow *w_;
    Alias a_;
    void run() override { w_->submitAlias(a_, false); } // async = false
  public:
    SubmitAlias(const Alias &a, MainWindow *w) : w_(w), a_(a) { Q_ASSERT(w_); }
  };

  class SetToken : public QRunnable
  {
    MainWindow *w_;
    QString t_;
    void run() override { w_->setToken(t_, false); } // async = false
  public:
    SetToken(const QString &t, MainWindow *w) : w_(w), t_(t) { Q_ASSERT(w_); }
  };

  class SignFileWithUrl : public QRunnable
  {
    MainWindow *w_;
    QString path_;
    QString url_;
    void run() override { w_->signFileWithUrl(path_, url_, false); } // async = false
  public:
    SignFileWithUrl(const QString &path, const QString &url, MainWindow *w)
      : w_(w), path_(path), url_(url) { Q_ASSERT(w_); }
  };


#define CAST(_cast, _Cast, _entity) \
  class _Cast##_entity##WithId : public QRunnable \
  { \
    MainWindow *w_; \
    qint64 id_; \
    void run() override { w_->_cast##_entity##WithId(id_, false); } \
  public: \
    _Cast##_entity##WithId(qint64 id, MainWindow *w) : w_(w), id_(id) { Q_ASSERT(w_); } \
  };

  CAST(bless, Bless, Token)      CAST(curse, Curse, Token)
  CAST(bless, Bless, User)       CAST(curse, Curse, User)       CAST(block, Block, User)
  CAST(bless, Bless, Alias)      CAST(curse, Curse, Alias)      CAST(block, Block, Alias)
  CAST(bless, Bless, Annotation) CAST(curse, Curse, Annotation) CAST(block, Block, Annotation)
#undef CAST

} // namespace detail

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
  bool eventFilter(QObject *hook, QEvent *event) override // Hook event filter.
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
