#ifndef GOOGLETRANSLATOR_P_H
#define GOOGLETRANSLATOR_P_H

// googletranslator.h
// 6/30/2012

#include "module/translator/googletranslator.h"
#include <QtCore/QTimer>

// - Slots -

namespace detail {

  class ProcessWebPage : public QObject
  {
    Q_OBJECT
    Q_DISABLE_COPY(ProcessWebPage)
    typedef QObject Base;

    GoogleTranslator *t_;
    QWebPage *page_;
  public:
    ProcessWebPage(QWebPage *page, GoogleTranslator *t)
      : Base(t), t_(t), page_(page) { }

    ~ProcessWebPage()
    { t_->releaseWebPage(page_); }

  public slots:
    void trigger(bool ok)
    {
      t_->processWebPage(page_, ok);
      QTimer::singleShot(0, this, SLOT(deleteLater()));
    }
  };

} // namespace detail

#endif // GOOGLETRANSLATOR_P_H
