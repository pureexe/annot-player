#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

// mainwindow_p.h
// 11/8/2011

#include "mainwindow.h"
#include <QtCore/QTimer>

namespace detail {
  // - Slots -

  class OpenUrls : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QStringList urls_;

  public:
    OpenUrls(const QStringList &urls, MainWindow *w)
      : Base(w), w_(w), urls_(urls)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->openUrls(urls_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

  class OpenUrl : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QString url_;

  public:
    OpenUrl(const QString &url, MainWindow *w)
      : Base(w), w_(w), url_(url)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->openUrl(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

  class NewTab : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QString url_;

  public:
    NewTab(const QString &url, MainWindow *w)
      : Base(w), w_(w), url_(url)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->newTab(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace detail

#endif // MAINWINDOW_P_H
