#ifndef _QTEXT_CRYPT_H
#define _QTEXT_CRYPT_H

// qtext/crypt.h
// 9/4/2011

#include <QByteArray>

namespace QtExt {

  namespace Crypt {

    QByteArray hex(const QByteArray &input);
    QByteArray sha1(const QByteArray &input);
    QByteArray md5(const QByteArray &input);

  } // namespace Crypt

} // namespace QtExt

#endif // _QTEXT_CRYPT_H
