// translator.cc
// 11/2/2011

#include "lib/translator/translator.h"
#include "lib/translator/translatorsettings.h"
#include <QtCore/QLocale>

// Helpers

// - Actions -

void
Translator::translate(const QString &text, int to, int from)
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
