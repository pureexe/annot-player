// annotcloud/alias.cc
// 10/29/2011

#include "module/annotcloud/alias.h"
#include "module/mrlanalysis/mrlanalysis.h"

using namespace AnnotCloud;

namespace { // anonymous
  struct init_ { init_() {
    qRegisterMetaType<Alias>("Alias");
    qRegisterMetaType<AliasList>("AliasList");
  } };
  init_ static_init_;
} // anonymois namespace

qint32
Alias::guessUrlLanguage(const QString &url, qint32 defval)
{
  if (url.contains(MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive))
    return Traits::Japanese;
  else if (url.contains(MA_EIGEN_YOUTUBE, Qt::CaseInsensitive))
    return defval;
  else
    return Traits::Chinese;
}

// EOF
