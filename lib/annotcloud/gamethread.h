#ifndef _ANNOTCLOUD_GAMETHREAD_H
#define _ANNOTCLOUD_GAMETHREAD_H

// annotcloud/gamethread.h
// 8/29/2012

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace AnnotCloud {

  class GameThread
  {
    typedef GameThread Self;

    // - Types -
  public:
    enum Field {
      None = -1,
      Id,
      TokenId, UserId,
      Flags, Type,
      UpdateTime, UpdateIp,
      Signature, Encoding, Provider,
      FieldCount
    };

    // - Properties -

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

  private: qint64 userId_;
  public:
    qint64 userId() const               { return userId_; }
    void setUserId(qint64 uid)          { userId_ = uid; }
    bool hasUserId() const              { return userId_; }

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

  private: qint32 type_;
  public:
    qint32 type() const                 { return type_; }
    void setType(qint32 type)           { type_ = type; }
    bool hasType() const                { return type_; }

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

  private: qint64 signature_;
  public:
    qint64 signature() const            { return signature_; }
    void setSignature(qint64 hash)      { signature_ = hash; }
    bool hasSignature() const           { return signature_; }

  private: qint32 encoding_;
  public:
    qint32 encoding() const             { return encoding_; }
    void setEncoding(qint32 codePage)   { encoding_ = codePage; }
    bool hasEncoding() const            { return encoding_; }

  private: QString provider_;
  public:
    const QString &provider() const     { return provider_; }
    void setProvider(const QString &t)  { provider_ = t; }
    bool hasProvider() const            { return !provider_.isEmpty(); }

    // - Constructions -
  public:
    GameThread()
      : id_(0), tokenId_(0), userId_(0), flags_(0), type_(0),
        updateTime_(0), updateIp_(0),
        signature_(0), encoding_(0)
    { }

    bool isValid() const { return hasId(); }
    void clear() { (*this) = Self(); }

    // - Operators -
  public:
    //bool operator==(const Self &that) { return !operator!=(that); }
    //bool operator!=(const Self &that) { return ::memcmp(this, &that, sizeof(Self)); }

    // Static helpers
  };

  typedef QList<GameThread> GameThreadList;

  // - Comparison -

  //inline bool
  //orderByAnnotationId(const Annotation &x, const Annotation &y)
  //{ return x.id() < y.id(); }
//
  //inline bool
  //orderByAnnotationPos(const Annotation &x, const Annotation &y)
  //{ return x.pos() < y.pos(); }

} // namespace AnnotCloud

using namespace AnnotCloud; // TO BE REMOVED!
Q_DECLARE_METATYPE(GameThread)
Q_DECLARE_METATYPE(GameThreadList)

#endif // _ANNOTCLOUD_GAMETHREAD_H
