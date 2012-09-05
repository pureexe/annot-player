// translatormanager.cc
// 7/1/2012

#include "module/translator/translatormanager.h"

#include "module/translator/excitetranslator.h"
#include "module/translator/fresheyetranslator.h"
#include "module/translator/googletranslator.h"
#include "module/translator/infoseektranslator.h"
#include "module/translator/microsofttranslator.h"
#include "module/translator/niftytranslator.h"
#include "module/translator/ocntranslator.h"
#include "module/translator/romajitranslator.h"
#include "module/translator/sdltranslator.h"
#include "module/translator/systrantranslator.h"
#include "module/translator/yahootranslator.h"
#include <boost/foreach.hpp>

// - Construction -

TranslatorManager::TranslatorManager(QObject *parent)
  : Base(parent), services_(0)
{
#define ADD(_service) \
  t = t_[_service] = new _service##Translator(this); \
  connect(t, SIGNAL(errorMessage(QString)), SIGNAL(errorMessage(QString))); \
  connect(t, SIGNAL(translated(QString)), SIGNAL(translated(QString)));

  Translator*
  ADD(Romaji)
  ADD(Microsoft)
  ADD(Google)
  ADD(Yahoo)
  ADD(Infoseek)
  ADD(Ocn)
  ADD(Fresheye)
  ADD(Nifty)
  ADD(Excite)
  ADD(Sdl)
  ADD(Systran)
#undef ADD

  connect(t_[Romaji], SIGNAL(translated(QString)), SIGNAL(translatedByRomaji(QString)));
  connect(t_[Microsoft], SIGNAL(translated(QString)), SIGNAL(translatedByMicrosoft(QString)));
  connect(t_[Google], SIGNAL(translated(QString)), SIGNAL(translatedByGoogle(QString)));
  connect(t_[Yahoo], SIGNAL(translated(QString)), SIGNAL(translatedByYahoo(QString)));
  connect(t_[Infoseek], SIGNAL(translated(QString)), SIGNAL(translatedByInfoseek(QString)));
  connect(t_[Ocn], SIGNAL(translated(QString)), SIGNAL(translatedByOcn(QString)));
  connect(t_[Fresheye], SIGNAL(translated(QString)), SIGNAL(translatedByFresheye(QString)));
  connect(t_[Nifty], SIGNAL(translated(QString)), SIGNAL(translatedByNifty(QString)));
  connect(t_[Excite], SIGNAL(translated(QString)), SIGNAL(translatedByExcite(QString)));
  connect(t_[Sdl], SIGNAL(translated(QString)), SIGNAL(translatedBySdl(QString)));
  connect(t_[Systran], SIGNAL(translated(QString)), SIGNAL(translatedBySystran(QString)));

  connect(this, SIGNAL(synchronizedChanged(bool)), SLOT(updateSynchronized(bool)));
}

QString
TranslatorManager::name() const
{ return tr("Honyaku"); }

void
TranslatorManager::updateSynchronized(bool t)
{
  BOOST_FOREACH (Translator *p, t_)
    p->setSynchronized(t);
}

// EOF
