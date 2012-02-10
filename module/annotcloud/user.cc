// annotcloud/user.cc
// 8/14/2011

#include "module/annotcloud/user.h"
#include "module/crypt/crypt.h"
#include <QtCore>

using namespace AnnotCloud;

#define GUEST_NAME      "guest"
#define GUEST_NICKNAME  "guest"
#define GUEST_PASSWORD  "guest"

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
AnnotCloud::
User::isLatin1String(const QString &s)
{ return s == s.toLatin1(); }

bool
AnnotCloud::
User::isValidName(const QString &userName)
{ return !userName.isEmpty() && isLatin1String(userName); }

bool
AnnotCloud::
User::isValidPassword(const QString &password)
{ return !password.isEmpty() && isLatin1String(password); }

QByteArray
AnnotCloud::
User::digest(const QByteArray &input)
{
  QByteArray ret = Crypt::hex(Crypt::sha1(input));
  return ret.toUpper();
}

// - Guest -

const User &
AnnotCloud::
User::guest()
{
  static User ret;
  if (!ret.hasId()) {
    ret.setId(UI_Guest);
    ret.setName(GUEST_NAME);
    ret.setNickname(GUEST_NICKNAME);
    ret.setPassword(encryptPassword(GUEST_PASSWORD));
  }
  return ret;
}

// EOF
