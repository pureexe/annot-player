#ifndef _ANNOTCLOUD_USER_H
#define _ANNOTCLOUD_USER_H

// annotcloud/user.h
// 8/14/2011

#include "module/annotcloud/traits.h"
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace AnnotCloud {

  class User
  {
    typedef User Self;

    // - Types -
  public:
    enum UserStatus {
      US_Invisible = 2,
      US_Online = 1,
      US_Offline = 0,
      US_Deleted = -1,
      US_Blocked = -2
    };

    enum UserFlag {
      UF_Anonymous = 0x1L
    };

    enum UserId {
      UI_Developer = 2,
      UI_Guest = 4
    };

    static const Self &guest();

    // - Properties -

    // id > 0: made by human; id < 0: made by doll
  private: qint64 id_;
  public:
    qint64 id() const                   { return id_; }
    void setId(qint64 id)               { id_ = id; }
    bool hasId() const                  { return id_; }

  //private: qint64 groupId_;
  //public:
  //  qint64 groupId() const              { return groupId_; }
  //  void setGroupId(qint64 gid)         { groupId_ = gid; }
  //  bool hasGroupId() const             { return groupId_; }

  private: QString name_;
  public:
    const QString &name() const         { return name_; }
    void setName(const QString &name)   { name_ = name; }
    bool hasName() const                { return !name_.isEmpty(); }
    bool hasValidName() const           { return isValidName(name_); }

  private: QString nickname_;
  public:
    const QString &nickname() const     { return nickname_; }
    void setNickname(const QString &name) { nickname_ = name; }
    bool hasNickname() const            { return !nickname_.isEmpty(); }

    // Password encoded as SHA1 hex string.
  private: QString password_;
  public:
    const QString &password() const     { return password_; }
    void setPassword(const QString &hex) { password_ = hex; }
    bool hasPassword() const            { return !password_.isEmpty(); }

  private: QString email_;
  public:
    const QString &email() const        { return email_; }
    void setEmail(const QString &email) { email_ = email; }
    bool hasEmail() const               { return !email_.isEmpty(); }

  private: qint32 status_;
  public:
    qint32 status() const               { return status_; }
    void setStatus(qint32 status)       { status_ = status; }

  private: quint64 flags_;
  public:
    quint64 flags() const               { return flags_; }
    void setFlags(quint64 flags)        { flags_ = flags; }

    bool hasFlag(quint64 bit) const     { return flags_ & bit; }

    void setFlag(quint64 bit, bool enabled = true)
    {
      if (enabled)
        flags_ |= bit;
      else
        flags_ &= ~bit;
    }

    bool isAnonymous() const            { return hasFlag(UF_Anonymous); }
    void setAnonymous(bool t = true)    { setFlag(UF_Anonymous, t); }

  private: qint32 language_;
  public:
    qint32 language() const             { return language_; }
    void setLanguage(qint32 lang)       { language_ = lang; }
    bool hasLanguage() const            { return language_; }

  private: qint64 createTime_;
  public:
    qint64 createTime() const           { return createTime_; }
    void setCreateTime(qint64 secs)     { createTime_ = secs; }
    bool hasCreateTime() const          { return createTime_ > 0; }

  private: qint64 createIp_;
  public:
    qint64 createIp() const              { return createIp_; }
    void setCreateIp(qint64 ip)          { createIp_ = ip; }
    bool hasCreateIp() const             { return createIp_; }

  private: qint64 loginTime_;
  public:
    qint64 loginTime() const            { return loginTime_; }
    void setLoginTime(qint64 secs)      { loginTime_ = secs; }
    bool hasLoginTime() const           { return loginTime_ > 0; }

  private: qint64 loginIp_;
  public:
    qint64 loginIp() const              { return loginIp_; }
    void setLoginIp(qint64 ip)          { loginIp_ = ip; }
    bool hasLoginIp() const             { return loginIp_; }

  private: qint64 loginCount_;
  public:
    qint64 loginCount() const           { return loginCount_; }
    qint64 &loginCount()                { return loginCount_; }
    void setLoginCount(qint64 count)    { loginCount_ = count; }

  private: quint32 blessed_;
  public:
    quint32 blessedCount() const        { return blessed_; }
    quint32 &blessedCount()             { return blessed_; }
    void setBlessedCount(quint32 count) { blessed_ = count; }
    bool isBlessed() const              { return blessed_; }

  private: quint32 cursed_;
  public:
    quint32 cursedCount() const         { return cursed_; }
    quint32 &cursedCount()              { return cursed_; }
    void setCursedCount(quint32 count)  { cursed_ = count; }
    bool isCursed() const               { return cursed_; }

  private: quint32 blocked_;
  public:
    quint32 blockedCount() const        { return blocked_; }
    quint32 &blockedCount()             { return blocked_; }
    void setBlockedCount(quint32 count) { blocked_ = count; }
    bool isBlocked() const              { return blocked_; }

  private: quint32 annot_;
  public:
    quint32 annotCount() const          { return annot_; }
    quint32 &annotCount()               { return annot_; }
    void setAnnotCount(quint32 count)   { annot_ = count; }
    bool isAnnotated() const            { return annot_; }

    // - Constructions -
  public:
    User()
      : id_(0), status_(0), flags_(0), language_(0),
        createTime_(0), createIp_(0), loginTime_(0), loginIp_(0),
        loginCount_(0), blessed_(0), cursed_(0), blocked_(0), annot_(0)
    { }

    bool isValid() const { return hasId(); }
    bool isGuest() const { return id() == UI_Guest; }
    bool isDeveloper() const { return id() == UI_Developer; }

    void clear() { (*this) = Self(); }

    // - Operators -
    bool operator==(const Self &that) { return !operator!=(that); }
    bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }

    // - Helpers -
  public:
    static bool isValidName(const QString &userName);
    static bool isValidPassword(const QString &password);
    static bool isValidNickName(const QString &nickName);

    static bool isLatin1String(const QString &latin1);

    static QString encryptUserName(const QString password)
    { return digest(password.toLocal8Bit()); }

    static QString encryptPassword(const QString password)
    { return digest(password.toLocal8Bit()); }

  protected:
    static QByteArray digest(const QByteArray &input);
  };

  typedef QList<User> UserList;

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(User)
Q_DECLARE_METATYPE(UserList)

#endif // _ANNOTCLOUD_USER_H
