#ifndef _CORE_CLOUD_TOKEN_H
#define _CORE_CLOUD_TOKEN_H

// core/cloud/token.h
// 8/7/2011

#include "core/cloud/traits.h"
#include <QString>
#include <QMetaType>
#include <QList>

namespace Core { namespace Cloud {

  class Token
  {
    typedef Token Self;

    // - Types -
  public:
    typedef Traits::Type Type;

    enum TokenStatus {
      TS_Active = 0,
      TS_Deleted = -1,
      TS_Blocked = -2
    };

    enum TokenFlag {
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

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserGId() const             { return userId_; }

  private: QString digest_;
  public:
    QString digest() const              { return digest_; }
    void setDigest(const QString &hex)  { digest_ = hex; }
    bool hasDigest() const              { return !digest_.isNull(); }

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
    void setBlockedCount(quint32 count)  { blocked_ = count; }
    bool isBlocked() const               { return blocked_; }

  private: quint32 visited_;
  public:
    quint32 visitedCount() const        { return visited_; }
    void setVisitedCount(quint32 count) { visited_ = count; }
    bool isVisited() const              { return visited_; }

    // - Constructions -
  public:
    Token()
      : type_(0), id_(0), userId_(0), status_(0), flags_(0), createTime_(0),
        blessed_(0), cursed_(0), blocked_(0), visited_(0)
    { }

    bool isValid() const { return hasId(); } //&& hasType

    void clear() { (*this) = Self(); }

    // - Helpers -
  public:
    static QString digestFromFile(const QString &fileName);
  };

  typedef QList<Token> TokenList;

} } // namespace Cloud, Core

using namespace Core::Cloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(Token);
Q_DECLARE_METATYPE(TokenList);

#endif // _CORE_CLOUD_TOKEN_H
