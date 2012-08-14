#ifndef MEDIA_H
#define MEDIA_H

// media.h
// 8/4/2012

#include "module/annotcloud/token.h"
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

class Media
{
  typedef Media Self;

  // - Types -
public:
  enum MediaType {
    None = AnnotCloud::Token::TT_Null,
    Url = AnnotCloud::Token::TT_Url,
    Video = AnnotCloud::Token::TT_Video,
    Audio = AnnotCloud::Token::TT_Audio,
    Picture = AnnotCloud::Token::TT_Picture,
    Game = AnnotCloud::Token::TT_Game
  };

  // - Operators -  required ordinary containers, including QSet
public:
  bool operator==(Self &that) const { return key() == that.key(); }

  // - Properties -

public:
  QIcon icon() const;

private:
  qint64 tokenId_;
public:
  qint64 tokenId() const { return tokenId_; }
  void setTokenId(qint64 id) { tokenId_ = id; }

private:
  int type_;
public:
  int type() const { return type_; }
  void setType(int type) { type_ = type; }

  bool isUrl() const { return type_ == Url; }
  bool isVideo() const { return type_ == Video; }
  bool isAudio() const { return type_ == Audio; }
  bool isPicture() const { return type_ == Picture; }
  bool isGame() const { return type_ == Game; }
  bool isMedia() const { return !isGame(); }

private:
  QString digest_;
public:
  const QString &digest() const { return digest_; }
  void setDigest(const QString &digest) { digest_ = digest; }
  bool hasDigest() const { return !digest_.isEmpty(); }

private:
  //mutable int remote_;
//public:
  //bool isRemote() const;

private:
  QString location_;
public:
  const QString &location() const { return location_; }
  void setLocation(const QString &location) { location_ = location; }
  bool hasLocation() const { return !location_.isEmpty(); }

private:
  bool exists_;
public:
  bool exists() const { return exists_; }
  void setExists(bool t) { exists_ = t; }

private:
  QString title_;
public:
  const QString &title() const { return title_; }
  void setTitle(const QString &title) { title_ = title; }
  bool hasTitle() const { return !title_.isEmpty(); }

private:
  qint64 visitTime_;
public:
  qint64 visitTime() const { return visitTime_; }
  void setVisitTime(qint64 msec) { visitTime_ = msec; }
  bool hasVisitTime() { return visitTime_; }

private:
  int visitCount_;
public:
  int visitCount() const { return visitCount_; }
  int &visitCount() { return visitCount_; }
  void setVisitCount(qint64 count) { visitCount_ = count; }
  bool hasVisitCount() { return visitCount_; }

  // - Construction -
public:
  Media() : tokenId_(0), type_(0), exists_(true), visitTime_(0), visitCount_(0) { }

  bool hasKey() const { return !digest_.isEmpty() || !location_.isEmpty(); }
  const QString &key() const { return digest_.isEmpty() ? location_ : digest_; }

  // - I/O -
public:
  template <typename L>
    static bool write(const L &l, const QString &fileName);

  static QList<Media> readList(const QString &fileName);
  static QHash<QString, Media> readHash(const QString &fileName);
};

QT_BEGIN_NAMESPACE
inline uint qHash(const Media &media) { return qHash(media.key()); }
QT_END_NAMESPACE

#endif // MEDIA_H
