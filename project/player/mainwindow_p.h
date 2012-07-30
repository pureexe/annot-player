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
