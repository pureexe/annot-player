#ifndef TEXTTHREAD_H
#define TEXTTHREAD_H

// textthread.h
// 8/27/2012
//
// TO BE MOVE TO ANNOTCLOUD
// text_thread
// - thread_id
// - thread_signature
// - thread_type
// - thread_provider
// - token_id
// - user_id
// - thread_update_ip
// - thread_update_time

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

class TextThread
{
  typedef TextThread Self;

  // - Properties -

private:
  int role_;
public:
  enum Role {
    NoRole = 0, // unitialized
    LeadingRole,
    SupportRole
  };
  int role() const { return role_; } // reserved
  void setRole(int value) { role_ = value; }
  bool hasRole() const { return role_; }

  bool isLeadingRole() const { return role_ == LeadingRole; }
  bool isSupportRole() const { return role_ == SupportRole; }

private:
  qint64 signature_;
public:
  qint64 signature() const { return signature_; }
  void setSignature(qint64 value) { signature_ = value; }
  bool hasSignature() const { return signature_; }

private:
  QString provider_;
public:
  const QString &provider() const { return provider_; }
  void setProvider(const QString &value) { provider_ = value; }
  bool hasProvider() const { return !provider_.isEmpty(); }

  // - Construction -
public:
  TextThread() : role_(0), signature_(0) { }
};

typedef QList<TextThread> TextThreadList;

Q_DECLARE_METATYPE(TextThread)
Q_DECLARE_METATYPE(TextThreadList)

#endif // TEXTTHREAD_H
