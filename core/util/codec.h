#ifndef _CORE_CODEC_H
#define _CORE_CODEC_H

#include <QObject>
#include <QString>
#include <QStringList>

// core/codec.h
// 7/18/2011
namespace Core {

  // - Codec -

  class CodecImpl;

  class Codec : public QObject
  {
    Q_OBJECT
    typedef Codec Self;
    typedef QObject Base;
    typedef CodecImpl Impl;

    // - Available encodings -
  public:
    enum Type { Default = 0, UTF8, SJIS, GBK };

    static const char *encoding(Type t);
    static QString description(Type t);
    static QStringList encodings();
    static QStringList descriptions();

    // - Constructions -
  public:
    explicit Codec(const QString &encoding, QObject *parent = 0);
    ~Codec();

    Type type() const { return type_; }
    void setType(Type type) { type_ = type; }

    const char *encoding() const { return encoding(type()); }
    QString description() const { return description(type()); }

    void setEncoding(const QString &encoding);

    bool valid() const { return impl_; }

    // - Decoding -
  public:
    QByteArray encode(const QString &unicode) const;
    QString decode(const char *ascii) const;

  private:
    Type type_;
    Impl *impl_;
  };

} // namespace Core
#endif // _CORE_CODEC_H
