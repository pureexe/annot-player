#ifndef _ANNOTCLOUD_TRAITS_H
#define _ANNOTCLOUD_TRAITS_H

// annotcloud/traits.h
// 8/17/2011

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>

namespace AnnotCloud {

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
    enum { AllLanguages =
      AnyLanguageBit | UnknownLanguageBit |
      EnglishBit | JapaneseBit | ChineseBit | KoreanBit
    };

    enum Entity {
      NullEntity = 0, // Error!
      User,
      Token,
      Alias,
      Annotation
    };
    enum { EntityCount = 4 };

    // - Limits -

    enum { MIN_TIME = 946706400 };  // QDateTime(QDate(2000, 1, 1)).toMSecsSinceEpoch() / 1000;
    enum { MAX_TIME = 1893477600 }; // QDateTime(QDate(2030, 1, 1)).toMSecsSinceEpoch() / 1000;

    // internal:
    enum { SHA1_DIGEST_LENGTH = 20 };
    enum { MD5_DIGEST_LENGTH = 16 };

    enum { MYSQL_MAX_VARCHAR_SIZE = 255 };

    // user:
    enum { SER_PASSWORD_LENGTH = SHA1_DIGEST_LENGTH * 2 };
    enum { MAX_USER_NAME_LENGTH = 64 };
    enum { MAX_USER_NICKNAME_LENGTH = 255 } ;
    enum { MAX_USER_EMAIL_LENGTH = 160 };

    // token:
    enum { TOKEN_DIGEST_LENGTH = MD5_DIGEST_LENGTH * 2 };
    enum { MAX_SOURCE_LENGTH = 255 };

    // alias:
    enum { MAX_ALIAS_LENGTH = 255 };

    // annot:
    enum { MAX_ANNOT_LENGTH = 255 };

    // misc:
    enum { MAX_SENDER_NAME_LENGTH = MAX_USER_NAME_LENGTH };

  } // namespace Concept

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_TRAITS_H
