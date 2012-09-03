// translator.cc
// 11/2/2011

#include "module/translator/translator.h"
#include "module/translator/translatorsettings.h"
#include <QtCore/QLocale>

// Helpers

// See: http://msdn.microsoft.com/en-us/library/hh456380.aspx
QString
Translator::languageCode(int lang, int script)
{
  switch (lang) {
  case QLocale::English:  return "en";
  case QLocale::Japanese: return "ja";
  case QLocale::Korean:   return "ko";
  case QLocale::French:   return "fr";
  case QLocale::German:   return "de";
  case QLocale::Italian:  return "it";
  case QLocale::Spanish:  return "es";
  case QLocale::Portuguese: return "pt";
  case QLocale::Russian:  return "ru";
  case QLocale::Chinese:  return script == QLocale::SimplifiedChineseScript ?
                                 "zh-CHS" : //"zh-CN" :
                                 "zh-CHT";  //"zh-TW" or "zh-HK";
  default: return QString();
  }
}

// - Actions -

void
Translator::translate(const QString &text, const QString &to, const QString &from)
{
  if (!isEnabled() || text.isEmpty())
    return;
  if (TranslatorSettings::globalSettings()->isCacheEnabled() &&
      text.size() < TranslatorSettings::globalSettings()->cacheSize() &&
      !cache_.isEmpty()) {
    auto p = cache_.find(text);
    if (p != cache_.end()) {
      emit translated(p.value());
      return;
    }
  }
  doTranslate(text, to, from);
}

// EOF
