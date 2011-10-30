#ifndef _CORE_CLOUD_ANNOTATION_H
#define _CORE_CLOUD_ANNOTATION_H

// core/cloud/annotation.h
// 7/24/2011

#include "core/cloud/traits.h"
#include "core/cloud/user.h"
#include <QString>
#include <QMetaType>
#include <QList>

namespace Core { namespace Cloud {

  class Annotation
  {
    typedef Annotation Self;

    // - Types -
  public:
    typedef Traits::Language Language;
    typedef Traits::Type Type;

    enum AnnotationStatus {
      AS_Active = 0,
      AS_Deleted = -1,
      AS_Blocked = -2
    };

    enum AnnotationFlag {
      AF_Anonymous = 0x1L
    };

    // - Properties -

  private: qint32 type_;
  public:
    qint32 type() const                 { return type_; }
    void setType(qint32 t)              { type_ = t; }
    bool hasType() const                { return type_; }

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
    QString tokenDigest() const             { return tokenDigest_; }
    void setTokenDigest(const QString &hex) { tokenDigest_ = hex; }
    bool hasTokenDigest() const             { return !tokenDigest_.isNull(); }

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserId() const              { return userId_; }

  private: QString userAlias_;
  public:
    QString userAlias() const           { return userAlias_; }
    void setUserAlias(const QString &alias) { userAlias_ = alias; }
    bool hasUserAlias() const           { return userAlias_.isEmpty(); }

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

  private: qint64 updateTime_;
  public:
    qint64 updateTime() const           { return updateTime_; }
    void setUpdateTime(qint64 secs)     { updateTime_ = secs; }
    bool hasUpdateTime() const          { return updateTime_ > 0; }

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

  private: QString text_;
  public:
    QString text() const                { return text_; }
    void setText(const QString &text)   { text_ = text; }
    bool hasText() const                { return !text_.isNull(); }

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
      : type_(0), id_(0), tokenId_(0), userId_(0), status_(0), flags_(0), language_(0),
        createTime_(0), updateTime_(0),
        pos_(0), posType_(0),
        blessed_(0), cursed_(0), blocked_(0)
    { }

    bool isValid() const { return hasId(); } // && hasType

    void clear() { (*this) = Self(); }

    // Static helpers
  public:
    static bool isValidText(const QString &text);

    static QString encryptUserName(const QString &name)
    { return digest(name.toAscii()); }

    static QString userAliasFromUserName(const QString &name)
    { return encryptUserName(name); }

    ///  Hash the text position.
    static qint64 hashTextPos(const QString &text)
    { return hash(text.toLocal8Bit()); }

    ///  Compute the text position incrementally from previous hash and added text.
    static qint64 rehashTextPos(const QString &text, qint64 h)
    { return rehash(text.toLocal8Bit(), h); }

  protected:
    static QByteArray digest(const QByteArray &input);

    static qint64 hash(const QByteArray &input);
    static qint64 rehash(const QByteArray &input, qint64 h);
  };

  typedef QList<Annotation> AnnotationList;

} } // namespace Cloud, Core

using namespace Core::Cloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Annotation);
Q_DECLARE_METATYPE(AnnotationList);

#endif // _CORE_CLOUD_ANNOTATION_H
