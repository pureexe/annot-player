#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

// mainwindow_p.h
// 11/8/2011

#include "mainwindow.h"
#include <QTimer>

namespace slot_ { // anonymous slot_

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
    void openUrls()
    {
      w_->openUrls(urls_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace slot_

#endif // MAINWINDOW_P_H
