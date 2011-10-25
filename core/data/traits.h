#ifndef _CORE_DATA_TRAITS_H
#define _CORE_DATA_TRAITS_H

// core/data/traits.h
// 8/17/2011

#include <QByteArray>
#include <QString>
#include <QList>

namespace Core { namespace Data {

  // TODO: Use template classes rather than a shared namespace
  namespace Traits
  {
    // - Types -

    enum Language {
      NoLanguage = 0,
      AnyLanguage = 0x1,
      UnknownLanguage = 0x2,
      English = 0x10,
      Japanese = 0x11,
      Chinese = 0x12,
      Korean = 0x13
    };
    enum { LanguageCount = 5 };

    enum LanguageBit {
      NoLanguageBit =   1L << NoLanguage,
      AnyLanguageBit =  1L << AnyLanguage,
      UnknownLanguageBit = 1L << UnknownLanguage,
      EnglishBit =      1L << English,
      JapaneseBit =     1L << Japanese,
      ChineseBit =      1L << Chinese,
      KoreanBit =       1L << Korean
    };

    enum Type {
      NoType = 0, // Error!
      MediaType,
      GameType
    };
    enum { TypeCount = 2 };

    // - Limits -

    // internal:
    enum { SHA1_DIGEST_LENGTH = 20 };
    enum { MD5_DIGEST_LENGTH = 16 };

    enum { MYSQL_MAX_VARCHAR_SIZE = 255 };

    // user:
    enum { SER_PASSWORD_LENGTH = SHA1_DIGEST_LENGTH * 2 };
    enum { MAX_USER_NAME_LENGTH = 64 - 1 };
    enum { MAX_USER_NICKNAME_LENGTH = 255 - 1 } ;
    enum { MAX_USER_EMAIL_LENGTH = 160 - 1 };

    // token:
    enum { TOKEN_DIGEST_LENGTH = MD5_DIGEST_LENGTH * 2 };

    // alias:
    enum { MAX_ALIAS_LENGTH = 255 - 1 };

    // annot:
    enum { MAX_ANNOT_LENGTH = 255 - 1 };

    // misc:
    enum { MAX_SENDER_NAME_LENGTH = MAX_USER_NAME_LENGTH };

  } // namespace Concept

} } // namespace Data, Core

#endif // _CORE_DATA_TRAITS_H
