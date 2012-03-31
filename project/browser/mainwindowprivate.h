#ifndef MAINWINDOWPRIVATE_H
#define MAINWINDOWPRIVATE_H

// mainwindowprivate.h
// 11/8/2011

#include "mainwindow.h"
#include <QTimer>

namespace slot_ { // anonymous slot_

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
    void openUrl()
    {
      w_->openUrl(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace slot_

#endif // MAINWINDOWPRIVATE_H
