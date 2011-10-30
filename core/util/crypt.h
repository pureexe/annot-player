#ifndef _CORE_CRYPT_H
#define _CORE_CRYPT_H

// core/util/crypt.h
// 9/4/2011

#include <QByteArray>

namespace Core {

  namespace Crypt {

    QByteArray hex(const QByteArray &input);
    QByteArray sha1(const QByteArray &input);
    QByteArray md5(const QByteArray &input);

  } // namespace Crypt

} // namespace Core

#endif // _CORE_UTIL_CRYPT_H
