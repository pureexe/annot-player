// translatormanager.cc
// 7/1/2012

#include "module/translator/translatormanager.h"
#include "module/translator/googletranslator.h"
#include "module/translator/microsofttranslator.h"
#include "module/translator/romajitranslator.h"

// - Construction -

TranslatorManager::TranslatorManager(QObject *parent)
  : Base(parent), services_(0)
{
#define ADD(_service) \
  t = t_[_service] = new _service##Translator(this); \
  connect(t, SIGNAL(error(QString)), SIGNAL(error(QString))); \
  connect(t, SIGNAL(translated(QString)), SIGNAL(translated(QString)));

  Translator*
  ADD(Microsoft)
  ADD(Google)
  ADD(Romaji)
#undef ADD

  connect(t_[Microsoft], SIGNAL(translated(QString)), SIGNAL(translatedByMicrosoft(QString)));
  connect(t_[Google], SIGNAL(translated(QString)), SIGNAL(translatedByGoogle(QString)));
  connect(t_[Romaji], SIGNAL(translated(QString)), SIGNAL(translatedByRomaji(QString)));
}

// EOF
