// annotcloud/user.cc
// 8/14/2011

#include "annotcloud/user.h"
#include "module/qtext/crypt.h"
#include <QtCore>

using namespace AnnotCloud;
using namespace QtExt;

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
AnnotCloud::User::
isLatin1String(const QString &s)
{ return s == s.toLatin1(); }

bool
AnnotCloud::User::
isValidName(const QString &userName)
{ return !userName.isEmpty() && isLatin1String(userName); }

bool
AnnotCloud::User::
isValidPassword(const QString &password)
{ return !password.isEmpty() && isLatin1String(password); }

QByteArray
AnnotCloud::User::
digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// EOF
