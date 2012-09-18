#ifndef _CORE_TEXTCODEC_H
#define _CORE_TEXTCODEC_H

#include <QtCore/QString>
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QTextCodec)
QT_FORWARD_DECLARE_CLASS(QTextDecoder)

// core/codec.h
// 7/18/2011
namespace Core {

  // - Codec -

  class TextCodec
  {
    typedef TextCodec Self;

    // - Available encodings -
  public:
    enum Type { Default = 0, UTF8, SJIS, GBK };

    static const char *encoding(Type t);
    static QString description(Type t);
    static QStringList encodings();
    static QStringList descriptions();

    // - Constructions -
  public:
    explicit TextCodec(const QString &encoding)
      : type_(Default), codec_(nullptr), decoder_(nullptr)
    { setEncoding(encoding); }

    explicit TextCodec(Type type)
      : type_(Default), codec_(nullptr), decoder_(nullptr)
    { setType(type); }

    // - Properties -
  public:
    Type type() const { return type_; }
    void setType(Type type);

    const char *encoding() const { return encoding(type()); }
    QString description() const { return description(type()); }

    void setEncoding(const QString &encoding);

    // - Decode -
  public:
    QByteArray encode(const QString &unicode) const;
    QString decode(const char *ascii) const;

    // - Implementation -
  protected:
    void updateCodec();

  private:
    Type type_;
    QTextCodec *codec_;
    QTextDecoder *decoder_;

  };

} // namespace Core

#endif // _CORE_TEXTCODEC_H
