// acapplication_tr.cc
// 9/4/2012
#include "project/common/acapplication.h"
#include "project/common/acrc.h"
#include "project/common/acsettings.h"
#ifdef WITH_MODULE_ANNOTCLOUD
# include "module/annotcloud/traits.h"
#endif // WITH_MODULE_ANNOTCLOUD
#include "module/mstypes/lcid.h"
#ifdef WITH_MODULE_QT
# include "module/qt/qtrc.h"
#endif // WITH_MODULE_QT
#include <QtCore/QTranslator>

#define DEBUG "acapplication_tr"
#include "module/debug/debug.h"

// i18n
namespace { namespace detail {
  inline QTranslator *acTranslator(int lang)
  {
    const char *qm;
    switch (lang) {
    case LCID_JA_JP: qm = ACRC_TR_JA; break;
    case LCID_ZH_TW: qm = ACRC_TR_ZH_TW; break;
    case LCID_ZH_CN: qm = ACRC_TR_ZH_CN; break;
    default: qm = nullptr;
    }
    if (qm) {
      QTranslator *t = new QTranslator(qApp);
      if (t->load(qm, QTRC_PREFIX_TR))
        return t;
      else
        delete t;
    }
    return nullptr;
  }

#ifdef WITH_MODULE_QT
  inline QTranslator *qtTranslator(int lang)
  {
    const char *qm;
    switch (lang) {
    case LCID_JA_JP: qm = "qt_ja"; break;
    case LCID_ZH_TW: qm = "qt_zh_TW"; break;
    case LCID_ZH_CN: qm = "qt_zh_CN"; break;
    default: qm = nullptr;
    }

    if (qm) {
      QTranslator *t = new QTranslator(qApp);
      if (t->load(qm, QTRC_PREFIX_TR))
        return t;
      else
        delete t;
    }
    return nullptr;
  }
#endif // WITH_MODULE_QT
} } // anonymous namespace detail

void
AcApplication::loadTranslations()
{
    int lang = AcSettings::globalSettings()->language();
    if (!lang
#ifdef WITH_MODULE_ANNOTCLOUD
        || !AnnotCloud::Traits::isKnownLanguage(lang)
#endif // WITH_MODULE_ANNOTCLOUD
        ) {
      const QLocale system = QLocale::system();
      lang = ::lcidFromLocaleLanguage(system.language(), system.script());
      if (!lang)
        lang = LCID_EN_US;
      AcSettings::globalSettings()->setLanguage(lang);
    }
    if (QTranslator *t = detail::acTranslator(lang))
      installTranslator(t);
#ifdef WITH_MODULE_QT
    if (QTranslator *t = detail::qtTranslator(lang))
      installTranslator(t);
#endif // WITH_MODULE_QT
}

// EOF
