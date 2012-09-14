#ifndef _ANNOTCLOUD_ALIAS_H
#define _ANNOTCLOUD_ALIAS_H

// alias.h
// 10/5/2011

#include "lib/annotcloud/traits.h"
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace AnnotCloud {

  class Alias
  {
    typedef Alias Self;

    // - Types -
  public:
    typedef Traits::Language Language;

    enum AliasType {
      AT_Null = 0, // invalid
      AT_Title = 1,
      AT_File = 2,
      AT_Tag = 3,
      AT_Url = 4,
      AT_Name = 5,
      AT_Folder = 6,
      AT_Brand = 7,
      AT_Count
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
  private: qint32 tokenSection_;
  public:
    qint32 tokenSection() const            { return tokenSection_; }
    void setTokenSection(qint32 section)   { tokenSection_ = section; }
    bool hasTokenSection() const           { return tokenSection_; }

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

  private: qint64 updateIp_;
  public:
    qint64 updateIp() const             { return updateIp_; }
    void setUpdateIp(qint64 ip)         { updateIp_ = ip; }
    bool hasUpdateIp() const            { return updateIp_; }

  private: quint32 blessCount_;
  public:
    quint32 blessCount() const          { return blessCount_; }
    quint32 &blessCount()               { return blessCount_; }
    void setBlessCount(quint32 count)   { blessCount_ = count; }
    bool hasBlessCount() const          { return blessCount_; }
    bool isBlessed() const              { return hasBlessCount(); }

  private: quint32 curseCount_;
  public:
    quint32 curseCount() const          { return curseCount_; }
    quint32 &curseCount()               { return curseCount_; }
    void setCurseCount(quint32 count)   { curseCount_ = count; }
    bool hasCurseCount() const          { return curseCount_; }
    bool isCursed() const               { return hasCurseCount(); }

  private: quint32 blockCount_;
  public:
    quint32 blockCount() const          { return blockCount_; }
    quint32 &blockCount()               { return blockCount_; }
    void setBlockCount(quint32 count)   { blockCount_ = count; }
    bool hasBlockCount() const          { return blockCount_; }
    bool isBlocked() const              { return hasBlockCount(); }

    // - Constructions -
  public:
    Alias()
      : id_(0), tokenId_(0), tokenSection_(0), userId_(0), type_(0),
        status_(0), flags_(0), language_(0), updateTime_(0), updateIp_(0),
        blessCount_(0), curseCount_(0), blockCount_(0)
    { }

    bool isValid() const { return hasId() && hasType(); }

    bool isUrl() const { return type() == AT_Url; }

    void clear() { (*this) = Self(); }

    // - Operators -
    //bool operator==(const Self &that) { return !operator!=(that); }
    //bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }

    // - Helpers -

    static qint32 guessUrlLanguage(const QString &url, qint32 defval = 0);
  };

  typedef QList<Alias> AliasList;

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Alias)
Q_DECLARE_METATYPE(AliasList)

#endif // _ANNOTCLOUD_ALIAS_H
