#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H

// textthread.h
// 8/27/2012

#include <QtCore/QByteArray>

class TextMessage
{
  typedef TextMessage Self;

  // - Properties -
private:
  qint64 signature_;
public:
  qint64 signature() const { return signature_; }
  void setSignature(qint64 value) { signature_ = value; }
  bool hasSignature() const { return signature_; }

private:
  QByteArray data_;
public:
  const QByteArray &data() const { return data_; }
  void setData(const QByteArray &value) { data_ = value; }
  bool hasData() const { return !data_.isEmpty(); }

  // - Construction -
public:
  TextMessage() : signature_(0) { }
  explicit TextMessage(const QByteArray &data, qint64 signature = 0) : signature_(signature), data_(data) { }
};

#endif // TEXTMESSAGE_H
