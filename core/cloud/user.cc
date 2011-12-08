// core/cloud/user.cc
// 8/14/2011

#include "core/cloud/user.h"
#include "core/util/crypt.h"
#include <QtCore>

using namespace Core::Cloud;

// - Meta types -

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<User>("User");
    qRegisterMetaType<UserList>("UserList");
  } };
  init_ static_init_;
} // anonymous namespace

// - Digest -

bool
Core::Cloud::User::
isLatin1String(const QString &s)
{ return s == s.toLatin1(); }

bool
Core::Cloud::User::
isValidName(const QString &userName)
{ return !userName.isEmpty() && isLatin1String(userName); }

bool
Core::Cloud::User::
isValidPassword(const QString &password)
{ return !password.isEmpty() && isLatin1String(password); }

QByteArray
Core::Cloud::User::
digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// EOF
