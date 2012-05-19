#ifndef WEBBROWSER_P_H
#define WEBBROWSER_P_H

// webbrowser_p.h
// 1/27/2012

#include "webbrowser.h"
#include "gui/wbcomboedit.h"
#include <QtWebKit/QWebView>
#include <QtGui>

// - Helpers -

namespace { // Anonyomous

  inline QString shortenText(const QString &text, int len = 40)
  {
    Q_ASSERT(len > 3);
    return len && text.size() < len ? text :
           text.left(len - 3).append("...");
  }

} // Anonyomous namespace

// - Daemons -

namespace daemon_ {

  class SetTabText : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QWidget *w_;
    QTabWidget *tab_;

  public:
    SetTabText(QTabWidget *t, QWidget *w)
      : Base(w), w_(w), tab_(t)
    {  Q_ASSERT(tab_); }

  public slots:
    void trigger(const QString &text)
    {
      int index = tab_->indexOf(w_);
      if (index >= 0) {
        tab_->setTabText(index, ::shortenText(text));
        tab_->setTabToolTip(index, text);
      }
    }
  protected:
    static QString shortenText(const QString &text)
    {
      enum { len = 40 };
      return len && text.size() < len ? text :
             text.left(len - 3).append("...");
    }
  };

  class SetTabIcon : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QWebView *w_;
    QTabWidget *tab_;

  public:
    SetTabIcon(QTabWidget *t, QWebView *w)
      : Base(w), w_(w), tab_(t)
    {  Q_ASSERT(w_); Q_ASSERT(tab_); }

  public slots:
    void trigger(bool ok) { if (ok) trigger(); }
    void trigger()
    {
      int index = tab_->indexOf(w_);
      if (index >= 0)
        tab_->setTabIcon(index, w_->icon());
    }
  };

  class SetAddressIcon : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QWebView *w_;
    QTabWidget *tab_;
    WbComboEdit *edit_;
    QString address_;

  public:
    SetAddressIcon(WbComboEdit *edit, const QString &address, QTabWidget *tab, QWebView *w)
      : Base(w), w_(w), tab_(tab), edit_(edit), address_(address)
    {  Q_ASSERT(w_); Q_ASSERT(edit_); Q_ASSERT(tab_); }

  public slots:
    void trigger(bool ok)
    {
      if (ok) {
        if (w_ == tab_->currentWidget())
          edit_->clearIcon();
        trigger();
        QTimer::singleShot(0, this, SLOT(deleteLater()));
      }
    }

    void trigger()
    {
      int index = edit_->findText(address_);
      if (index >= 0)
        edit_->setItemIcon(index, w_->icon());
    }
  };

  class SearchTab : public QObject {
    Q_OBJECT
    typedef QObject Base;

    QWebView *w_;
    QTabWidget *tab_;
    WebBrowser *main_;

  public:
    SearchTab(WebBrowser *main, QTabWidget *tab, QWebView *w)
      : Base(w), w_(w), tab_(tab), main_(main)
    {  Q_ASSERT(w_); Q_ASSERT(tab_); Q_ASSERT(main_); }

  public slots:
    void trigger(bool ok)
    {
      if (ok && w_ == tab_->currentWidget())
        main_->invalidateSearch();
    }
  };

} // namespace daemon_

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
      return qMax<int>(MinCount, ret);
    }
  };

  */
