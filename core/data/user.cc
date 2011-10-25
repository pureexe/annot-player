// core/data/user.cc
// 8/14/2011

#include "core/data/user.h"
#include "core/util/crypt.h"
#include <QtCore>

bool
Core::Data::User::
isLatin1String(const QString &s)
{ return s == s.toLatin1(); }

bool
Core::Data::User::
isValidName(const QString &userName)
{ return !userName.isEmpty() && isLatin1String(userName); }

bool
Core::Data::User::
isValidPassword(const QString &password)
{ return !password.isEmpty() && isLatin1String(password); }

bool
Core::Data::User::
isValidEmail(const QString &email)
{ return !email.isEmpty() && isLatin1String(email); }

QByteArray
Core::Data::User::
digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// EOF
