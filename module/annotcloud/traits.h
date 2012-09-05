#ifndef _ANNOTCLOUD_TRAITS_H
#define _ANNOTCLOUD_TRAITS_H

// annotcloud/traits.h
// 8/17/2011

#include "module/mstypes/codepage.h"
#include "module/mstypes/lcid.h"
#include <QtCore/QString>

//#define AC_LANG(ch1, ch2)   (int(ch1) << 8 | (ch2))

namespace AnnotCloud {

  // TODO: Use template classes rather than a shared namespace
  namespace Traits
  {
    enum CodePage {
      NoCodePage = 0,
      CP_Utf8 = CODEPAGE_UTF8,
      CP_Utf16 = CODEPAGE_UTF16,
      CP_ShiftJIS = CODEPAGE_SHIFT_JIS,
      CP_GBK = CODEPAGE_GB2312,
      CP_EUC_KR = CODEPAGE_KS_C,
      CP_Big5 = CODEPAGE_BIG5
    };

    inline const char *codePageToEncoding(int value)
    {
      switch (value) {
      case CP_Utf8:   return "UTF-8";
      case CP_Utf16:  return "UTF-16";
      case CP_ShiftJIS: return "SHIFT-JIS";
      case CP_GBK:    return "GBK";
      case CP_EUC_KR: return "EUC-KR";
      case CP_Big5:   return "BIG5";
      default: return 0;
      }
    }

    inline CodePage codePageFromEncoding(const QString &value)
    {
      return !value.compare(value, "SHIFT-JIS", Qt::CaseInsensitive) ? CP_ShiftJIS :
             !value.compare(value, "UTF-16", Qt::CaseInsensitive) ? CP_Utf16 :
             !value.compare(value, "UTF-8", Qt::CaseInsensitive) ? CP_Utf8 :
             !value.compare(value, "BIG5", Qt::CaseInsensitive) ? CP_Big5 :
             !value.compare(value, "GBK", Qt::CaseInsensitive) ? CP_GBK :
             !value.compare(value, "ENC-KR", Qt::CaseInsensitive) ? CP_EUC_KR :
             NoCodePage;
    }

    // - Types -

    //enum Language {
    //  NoLanguage = 0,
    //  AnyLanguage = 0x1,
    //  UnknownLanguage = 0x2,
    //  English = 0x10,
    //  Japanese = 0x11,
    //  Chinese = 0x12,
    //  Korean = 0x13,
    //  French = 0x14,
    //  German = 0x15,
    //  Italian = 0x16,
    //  Spanish = 0x17,
    //  Portuguese = 0x18,
    //  Russian = 0x19
    //};
    //enum { LanguageCount = 9 };

    // Consistent with  ISO 639-1.
    // See: http://msdn.microsoft.com/en-us/library/hh456380.aspx
    enum Language {
      NoLanguage = 0,
      //AnyLanguage = 0x1,
      //UnknownLanguage = 0x2,
      English = LCID_EN_US,
      Japanese = LCID_JA_JP,
      TraditionalChinese = LCID_ZH_TW,
      SimplifiedChinese = LCID_ZH_CN,
      Korean = LCID_KO_KR,
      French = LCID_FR,
      German = LCID_DE,
      Italian = LCID_IT,
      Spanish = LCID_ES,
      Portuguese = LCID_PT,
      Russian = LCID_RU
    };

    // TO BE REMOVED!!!
    //  English = AC_LANG('e','n'),   // 25966 / 16
    //  Japanese = AC_LANG('j','a'),  // 27233 / 17
    //  Chinese = AC_LANG('z','h'),   // 31336 / 18
    //  Korean = AC_LANG('k','o'),    // 27503 / 19
    //  French = AC_LANG('f','r'),    // 26226 / 20
    //  German = AC_LANG('d','e'),    // 25701 / 21
    //  Italian = AC_LANG('i','t'),   // 26996 / 22
    //  Spanish = AC_LANG('e','s'),   // 25971 / 23
    //  Portuguese = AC_LANG('p','t'),// 28788 / 24
    //  Russian = AC_LANG('r','u')    // 29301 / 25

    inline bool isChineseLanguage(int lang)
    { return lang == TraditionalChinese || lang == SimplifiedChinese; }

    inline bool isKnownLanguage(int lang)
    {
      switch (lang) {
      case English:
      case Japanese:
      case TraditionalChinese:
      case SimplifiedChinese:
      case Korean:
      case French:
      case German:
      case Italian:
      case Spanish:
      case Portuguese:
      case Russian: return true;
      default:      return false;
      }
    }

    inline QByteArray languageCode(int lang)
    {
      QByteArray ret(2, 0);
      ret[0] = char((lang >> 8) & 0xff);
      ret[1] = char(lang & 0xff);
      return ret;
    }

    inline QLocale::Language localeLanguage(int lang)
    { return ::lcidToLocaleLanguage(lang); }

    inline bool isRomanLanguage(int lang)
    {
      switch (lang) {
      case English:
      case French: case German:
      case Italian:
      case Spanish:case Portuguese:
      case Russian:
        return true;
      default: return false;
      }
    }

    inline bool isAsianLanguage(int lang)
    {
      switch (lang) {
      case Japanese:
      case TraditionalChinese:
      case SimplifiedChinese:
      case Korean:
        return true;
      default: return false;
      }
    }

    /*
    enum LanguageBit {
      NoLanguageBit =   1L << NoLanguage,
      AnyLanguageBit =  1L << AnyLanguage,
      UnknownLanguageBit = 1L << UnknownLanguage,
      EnglishBit =      1L << English,
      JapaneseBit =     1L << Japanese,
      ChineseBit =      1L << Chinese,
      KoreanBit =       1L << Korean,
      FrenchBit =       1L << French,
      GermanBit =       1L << German,
      ItalianBit =      1L << Italian,
      SpanishBit =      1L << Spanish,
      PortugueseBit =   1L << Portuguese,
      RussianBit =      1L << Russian
    };
    enum { AllLanguages =
      AnyLanguageBit | UnknownLanguageBit |
      EnglishBit | JapaneseBit | ChineseBit | KoreanBit |
      FrenchBit | GermanBit | ItalianBit | SpanishBit | PortugueseBit | RussianBit
    };
    */

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
    enum { MAX_USER_PASSWORD_LENGTH = SHA1_DIGEST_LENGTH * 2 };
    enum { MAX_USER_NAME_LENGTH = 64 };
    enum { MAX_USER_NICKNAME_LENGTH = 255 } ;
    enum { MAX_USER_EMAIL_LENGTH = 160 };

    // token:
    enum { TOKEN_DIGEST_LENGTH = MD5_DIGEST_LENGTH * 2 };
    enum { MAX_URL_LENGTH = 255 };

    // alias:
    enum { MAX_ALIAS_LENGTH = 255 };

    // annot:
    enum { MAX_ANNOT_LENGTH = 255 };

    // misc:
    enum { MAX_SENDER_NAME_LENGTH = MAX_USER_NAME_LENGTH };

  } // namespace Concept

} // namespace AnnotCloud

#endif // _ANNOTCLOUD_TRAITS_H
