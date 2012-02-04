#ifndef _ANNOTCLOUD_ALIAS_H
#define _ANNOTCLOUD_ALIAS_H

// annotcloud/tokenalias.h
// 10/5/2011

#include "annotcloud/traits.h"
#include <QString>
#include <QList>
#include <QMetaType>

namespace AnnotCloud {

  class Alias
  {
    typedef Alias Self;

    // - Types -
  public:
    typedef Traits::Language Language;

    enum AliasType {
      AT_Null = 0, // invalid
      AT_Name = 1,
      AT_Source = 2,
      AT_Tag = 3,
      AT_Url = 4
    };

    enum AliasStatus {
      AS_Active = 0,
      AS_Deleted = -1,
      AS_Blocked = -2
    };

    enum AliasFlag {
    };

    // - Properties -

  //private: qint32 type_;
  //public:
  //  qint32 type() const                 { return type_; }
  //  void setType(qint32 t)              { type_ = t; }
  //  bool hasType() const                { return type_; }

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
    bool hasTokenDigest() const             { return !tokenDigest_.isEmpty(); }

    ///  Used only in offline mode
  private: qint32 tokenPart_;
  public:
    qint32 tokenPart() const            { return tokenPart_; }
    void setTokenPart(qint32 part)      { tokenPart_ = part; }
    bool hasTokenPart() const           { return tokenPart_; }

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserId() const              { return userId_; }

  private: int type_;
  public:
    int type() const                    { return type_; }
    void setType(int at)                { type_ = at; }
    bool hasType() const                { return type_; }

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

  private: qint32 language_;
  public:
    qint32 language() const            { return language_; }
    void setLanguage(qint32 lang)      { language_ = lang; }
    bool hasLanguage() const           { return language_; }

  private: QString text_;
  public:
    QString text() const                { return text_; }
    void setText(const QString &text)   { text_ = text; }
    bool hasText() const                { return !text_.isEmpty(); }

  private: qint64 updateTime_;
  public:
    qint64 updateTime() const           { return updateTime_; }
    void setUpdateTime(qint64 secs)     { updateTime_ = secs; }
    bool hasUpdateTime() const          { return updateTime_ > 0; }

  private: quint32 blessed_;
  public:
    quint32 blessedCount() const        { return blessed_; }
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

    // - Constructions -
  public:
    Alias()
      : id_(0), tokenId_(0), tokenPart_(0), userId_(0), type_(0),
        status_(0), flags_(0), language_(0), updateTime_(0),
        blessed_(0), cursed_(0), blocked_(0)
    { }

    bool isValid() const { return hasId() && hasType(); }

    void clear() { (*this) = Self(); }

    // - Operators -
    bool operator==(const Self &that) { return !operator!=(that); }
    bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }
  };

  typedef QList<Alias> AliasList;

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Alias)
Q_DECLARE_METATYPE(AliasList)

#endif // _ANNOTCLOUD_ALIAS_H
