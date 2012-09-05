#ifndef LCID_H
#define LCID_H

// lcid.h
// 9/1/2012
// See: http://download.microsoft.com/download/9/5/E/95EF66AF-9026-4BB0-A41D-A4F81802D92C/%5BMS-LCID%5D.pdf

#define LCID_NULL   0x0

#define LCID_ZH_TW  0x0404
#define LCID_ZH_CN  0x0804

#define LCID_EN_US  0x0409
//#define LCID_EN LCID_EN_US

#define LCID_JA_JP  0x0411
//#define LCID_JA LCID_JA_JP

#define LCID_KO_KR  0x0412
//#define LCID_KO LCID_KO_KR

#define LCID_DE_DE  0x0407
#define LCID_DE LCID_DE_DE

#define LCID_IT_IT  0x0410
#define LCID_IT LCID_IT_IT

#define LCID_RU_RU  0x0419
#define LCID_RU LCID_RU_RU

#define LCID_FR_FR  0x040C
#define LCID_FR LCID_FR_FR

#define LCID_PT_PT  0x0816
#define LCID_PT LCID_PT_PT

#define LCID_ES_ES  0x0C0A
#define LCID_ES LCID_ES_ES

inline const char *lcidToString(int lcid)
{
  switch (lcid) {
  case LCID_ZH_TW: return "zh-TW";
  case LCID_ZH_CN: return "zh-CN";
  case LCID_EN_US: return "en-US";
  case LCID_JA_JP: return "ja-JP";
  case LCID_KO_KR: return "ko-KR";
  case LCID_DE_DE: return "de-DE";
  case LCID_IT_IT: return "it-IT";
  case LCID_RU_RU: return "ru-RU";
  case LCID_FR_FR: return "fr-FR";
  case LCID_PT_PT: return "pt-PT";
  case LCID_ES_ES: return "es-ES";
  default: return 0;
  }
}

#ifdef QT_CORE_LIB
#include <QtCore/QLocale>

inline QLocale::Language lcidToLocaleLanguage(int lcid)
{
  switch (lcid) {
  case LCID_EN_US: return QLocale::English;
  case LCID_JA_JP: return QLocale::Japanese;
  case LCID_KO_KR: return QLocale::Korean;
  case LCID_FR_FR: return QLocale::French;
  case LCID_DE_DE: return QLocale::German;
  case LCID_IT_IT: return QLocale::Italian;
  case LCID_ES_ES: return QLocale::Spanish;
  case LCID_PT_PT: return QLocale::Portuguese;
  case LCID_RU_RU: return QLocale::Russian;
  case LCID_ZH_TW:
  case LCID_ZH_CN: return QLocale::Chinese;
  default:         return QLocale::AnyLanguage;
  }
}

inline int lcidFromLocaleLanguage(int lang, int script = 0)
{
  switch (lang) {
  case QLocale::English: return LCID_EN_US;
  case QLocale::Japanese: return LCID_JA_JP;
  case QLocale::Korean: return LCID_KO_KR;
  case QLocale::French: return LCID_FR_FR;
  case QLocale::German: return LCID_DE_DE;
  case QLocale::Italian: return LCID_IT_IT;
  case QLocale::Spanish: return LCID_ES_ES;
  case QLocale::Portuguese: LCID_PT_PT;
  case QLocale::Russian: return LCID_RU_RU;
  case QLocale::Chinese: return script == QLocale::SimplifiedChineseScript ? LCID_ZH_CN : LCID_ZH_TW;
  default: return LCID_NULL;
  }
}
#endif // QT_CORE_LIB

#endif // LCID_H
