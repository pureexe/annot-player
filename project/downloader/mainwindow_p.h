#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

// mainwindow_p.h
// 11/8/2011

#include "mainwindow.h"
#include <QtCore/QTimer>

namespace slot_ { // anonymous slot_

  class PromptUrl : public QObject {
    Q_OBJECT
    typedef QObject Base;

    MainWindow *w_;
    QString url_;

  public:
    PromptUrl(const QString &url, MainWindow *w)
      : Base(w), w_(w), url_(url)
    { Q_ASSERT(w_); }

  public slots:
    void trigger()
    {
      w_->promptUrl(url_);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace slot_

#endif // MAINWINDOW_P_H
