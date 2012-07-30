#ifndef _ANNOTCLOUD_ANNOTATION_H
#define _ANNOTCLOUD_ANNOTATION_H

// annotcloud/annotation.h
// 7/24/2011

#include "module/annotcloud/annottag.h"
#include "module/annotcloud/traits.h"
#include "module/annotcloud/user.h"
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <cstring>

namespace AnnotCloud {

  class Annotation
  {
    typedef Annotation Self;

    // - Types -
  public:
    enum AnnotationStatus {
      AS_Active = 0,
      AS_Deleted = -1,
      AS_Blocked = -2,
      AS_Hidden = -3
    };

    enum AnnotationFlag {
      AF_Anonymous = 0x1L
    };

    enum Field {
      None = -1,
      Id,
      TokenId, TokenDigest, TokenSection,
      UserId, UserAlias,
      Status, Flags, Language,
      CreateTime, UpdateTime,
      Pos, PosType,
      Time, Text,
      BlessedCount, CursedCount, BlockedCount,
      FieldCount
    };

    // - Properties -

  //private: qint32 type_;
  //public:
  //  qint32 type() const                 { return type_; }
  //  void setType(qint32 t)              { type_ = t; }
  //  bool hasType() const                { return type_; }

    // id > 0: made by human; id < 0: made by doll
  private: qint64 id_;
  public:
    qint64 id() const                   { return id_; }
    void setId(qint64 id)               { id_ = id; }
    bool hasId() const                  { return id_; }

  private: qint64 tokenId_;
  public:
    qint64 tokenId() const              { return tokenId_; }
    void setTokenId(qint64 tid)         { tokenId_ = tid; }
    bool hasTokenId() const             { return tokenId_; }

    ///  Used only in offline mode
  private: QString tokenDigest_;
  public:
    const QString &tokenDigest() const      { return tokenDigest_; }
    void setTokenDigest(const QString &hex) { tokenDigest_ = hex; }
    bool hasTokenDigest() const             { return !tokenDigest_.isEmpty(); }

    ///  Used only in offline mode
  private: qint32 tokenSection_;
  public:
    qint32 tokenSection() const          { return tokenSection_; }
    void setTokenSection(qint32 section) { tokenSection_ = section; }
    bool hasTokenSection() const         { return tokenSection_; }

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserId() const              { return userId_; }

  private: QString userAlias_;
  public:
    const QString &userAlias() const    { return userAlias_; }
    void setUserAlias(const QString &alias) { userAlias_ = alias; }
    bool hasUserAlias() const           { return !userAlias_.isEmpty(); }

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

    bool isUserAnonymous() const { return hasFlag(AF_Anonymous); }
    void setUserAnonymous(bool enabled) { setFlag(AF_Anonymous, enabled); }

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
    qint64 createIp() const             { return createIp_; }
    void setCreateIp(qint64 ip)         { createIp_ = ip; }
    bool hasCreateIp() const            { return createIp_; }

  private: qint64 updateTime_;
  public:
    qint64 updateTime() const           { return updateTime_; }
    void setUpdateTime(qint64 secs)     { updateTime_ = secs; }
    bool hasUpdateTime() const          { return updateTime_ > 0; }

  private: qint64 updateIp_;
  public:
    qint64 updateIp() const             { return updateIp_; }
    void setUpdateIp(qint64 ip)         { updateIp_ = ip; }
    bool hasUpdateIp() const            { return updateIp_; }

  private: qint64 pos_;
  public:
    qint64 pos() const                  { return pos_; }
    void setPos(qint64 pos)             { pos_ = pos; }
    bool hasPos() const                 { return pos_ >= 0; }

  private: int posType_;
  public:
    /**
     *  Indicate the type of pos field.
     *  - For media type, this value is ignored
     *  - For game type, this value indicate number of sentence used to hash the pos
     */
    int posType() const                 { return posType_; }
    void setPosType(int count)          { posType_ = count; }
    bool hasPosType() const             { return posType_; }

  private: int time_;
  public:
    int time() const                 { return time_; }
    void setTime(int msecs)          { time_ = msecs; }
    bool hasTime() const             { return time_; }

  private: QString text_;
  public:
    const QString &text() const         { return text_; }
    void setText(const QString &text)   { subtitle_ = -1; text_ = text; }
    bool hasText() const                { return !text_.isEmpty(); }

  private: quint32 blessed_;
  public:
    quint32 blessedCount() const        { return blessed_; }
    void setBlessedCount(quint32 count) { blessed_ = count; }
    bool isBlessed() const              { return blessed_; }

  private: quint32 cursed_;
  public:
    quint32 cursedCount() const         { return cursed_; }
    void setCursedCount(quint32 count)  { cursed_ = count; }
    bool isCursed() const               { return cursed_; }

  private: quint32 blocked_;
  public:
    quint32 blockedCount() const        { return blocked_; }
    void setBlockedCount(quint32 count) { blocked_ = count; }
    bool isBlocked() const               { return blocked_; }

    // - Constructions -
  public:
    Annotation()
      : id_(0), tokenId_(0), tokenSection_(0), userId_(0), status_(0), flags_(0), language_(0),
        createTime_(0), createIp_(0), updateTime_(0), updateIp_(0),
        pos_(0), posType_(0), time_(0),
        blessed_(0), cursed_(0), blocked_(0), subtitle_(-1)
    { }

    bool isValid() const { return hasId(); }
    bool isLive() const { return id() < 0; }
    bool isVisible() const { return status() >= 0; }
    bool isHidden() const { return status() < 0; }


    void clear() { (*this) = Self(); }

    bool isSubtitle() const
    {
      switch (subtitle_) {
      case 0: return false;
      case 1: return true;
      case -1:
        if (hasText() && text().contains(CORE_CMD_SUB)) {
          subtitle_ = 1;
          return true;
        } else {
          subtitle_ = 0;
          return false;
        } Q_ASSERT(0);
      default: Q_ASSERT(0);
        return false;
      }
    }
  private:
    mutable int subtitle_;

    // - Operators -
  public:
    bool operator==(const Self &that) { return !operator!=(that); }
    bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }

    // Static helpers
  public:
    static bool isValidText(const QString &text);

    static QString encryptUserName(const QString &name)
    { return digest(name.toLocal8Bit()); }

    static QString userAliasFromUserName(const QString &name)
    { return encryptUserName(name); }

    ///  Hash the text position.
    static qint64 hashTextPos(const QString &text)
    { return hash(text.toLocal8Bit()); }

    ///  Compute the text position incrementally from previous hash and added text.
    static qint64 rehashTextPos(const QString &text, qint64 h)
    { return rehash(text.toLocal8Bit(), h); }

    static qint64 hashTextsPos(const QStringList &l)
    {
      qint64 h;
      if (l.isEmpty())
        h = hash(QByteArray());
      else for (int i = 0; i < l.size(); i++)
        if (i == 0)
          h = hashTextPos(l.first());
        else
          h = rehashTextPos(l[i], h);
      return h;
    }

  public:
    static QByteArray digest(const QByteArray &input);

    static qint64 hash(const QByteArray &input);
    static qint64 rehash(const QByteArray &input, qint64 h);
    static qint64 hash(const QList<QByteArray> &l);
  };

  typedef QList<Annotation> AnnotationList;

  // - Comparison -

  inline bool
  orderByAnnotationId(const Annotation &x, const Annotation &y)
  { return x.id() < y.id(); }

  inline bool
  orderByAnnotationPos(const Annotation &x, const Annotation &y)
  { return x.pos() < y.pos(); }

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Annotation)
Q_DECLARE_METATYPE(AnnotationList)

#endif // _ANNOTCLOUD_ANNOTATION_H
