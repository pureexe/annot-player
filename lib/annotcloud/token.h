#ifndef _ANNOTCLOUD_TOKEN_H
#define _ANNOTCLOUD_TOKEN_H

// annotcloud/token.h
// 8/7/2011

//#include "lib/annotcloud/traits.h"
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace AnnotCloud {

  class Token
  {
    typedef Token Self;

    // - Types -
  public:
    enum TokenType {
      TT_Null = 0x0,
      TT_Url = 0x7,
      TT_Video = 0x8,
      TT_Audio = 0x9,
      TT_Picture = 0xa,
      TT_Game = 0xb
    };

    enum TokenStatus {
      TS_Active = 0,
      TS_Deleted = -1,
      TS_Blocked = -2
    };

    enum TokenFlag {
    };

    enum TokenId {
      TI_Public = -1L
    };

    // - Properties -

    // id > 0: made by human; id < 0: made by doll
  private: qint64 id_;
  public:
    qint64 id() const                   { return id_; }
    void setId(qint64 id)               { id_ = id; }
    bool hasId() const                  { return id_; }

  private: qint32 type_;
  public:
    qint32 type() const                 { return type_; }
    void setType(qint32 t)              { type_ = t; }
    bool hasType() const                { return type_; }

    bool isUrl() const                  { return type_ == TT_Url; }
    bool isVideo() const                { return type_ == TT_Video; }
    bool isAudio() const                { return type_ == TT_Audio; }
    bool isPicture() const              { return type_ == TT_Picture; }
    bool isGame() const                 { return type_ == TT_Game; }
    bool isMedia() const { return !isGame(); }

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserGId() const             { return userId_; }

  private: QString digest_;
  public:
    const QString &digest() const       { return digest_; }
    void setDigest(const QString &hex)  { digest_ = hex; }
    bool hasDigest() const              { return !digest_.isEmpty(); }

  private: QString url_;
  public:
    const QString &url() const          { return url_; }
    void setUrl(const QString &href)    { url_ = href; }
    bool hasUrl() const                 { return !url_.isEmpty(); }

  private: qint32 section_;
  public:
    qint32 section() const              { return section_; }
    void setSection(qint32 section)     { section_ = section; }
    bool hasSection() const             { return section_; }

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

  private: quint32 visitCount_;
  public:
    quint32 visitCount() const          { return visitCount_; }
    quint32 &visitCount()               { return visitCount_; }
    void setVisitCount(quint32 count)   { visitCount_ = count; }
    bool hasVisitCount() const          { return visitCount_; }
    bool isVisited() const              { return hasVisitCount(); }

  private: quint32 annotCount_;
  public:
    quint32 annotCount() const          { return annotCount_; }
    quint32 &annotCount()               { return annotCount_; }
    void setAnnotCount(quint32 count)   { annotCount_ = count; }
    bool hasAnnotCount() const          { return annotCount_; }
    bool isAnnotated() const            { return hasAnnotCount(); }

    // - Constructions -
  public:
    Token()
      : id_(0), type_(0), userId_(0), section_(0), status_(0), flags_(0), createTime_(0), createIp_(0),
        blessCount_(0), curseCount_(0), blockCount_(0), visitCount_(0), annotCount_(0)
    { }

    bool isValid() const { return hasId(); } //&& hasType
    bool isLive() const { return id() < 0; }
    bool isRemote() const { return hasUrl(); }

    void clear() { (*this) = Self(); }

    qint64 hashId() const
    {
      return hasId() ? id() :
             hasDigest() ?  -qint64(qHash(digest()) + section()) :
             hasUrl() ? -qint64(qHash(url()) + section()) :
             0LL;
    }

    // - Operators -

    //bool operator==(const Self &that) { return !operator!=(that); }
    //bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }

    // - Helpers -
  public:
    static QString digestFromFile(const QString &fileName);
  };

  typedef QList<Token> TokenList;

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Token)
Q_DECLARE_METATYPE(TokenList)

#endif // _ANNOTCLOUD_TOKEN_H
