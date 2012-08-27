// networktranslator.cc
// 8/26/2011

#include "module/translator/networktranslator.h"

#include <QtNetwork/QNetworkAccessManager>

// - Properties -

QNetworkAccessManager*
NetworkTranslator::networkAccessManager() const
{
  if (!nam_) {
    Self *self = const_cast<Self *>(this);
    self->nam_ = new QNetworkAccessManager(self);
    connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(processReply(QNetworkReply*)));
  }
  return nam_;
}

// EOF
