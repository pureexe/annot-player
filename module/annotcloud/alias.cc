// annotcloud/alias.cc
// 10/29/2011

#include "module/annotcloud/alias.h"

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
  qint32 ret = defval;
  if (url.contains(".nicovideo.jp/", Qt::CaseInsensitive))
    ret = Traits::Japanese;
  else if (url.contains(".bilibili.tv/", Qt::CaseInsensitive) ||
           url.contains(".acfun.tv/", Qt::CaseInsensitive) ||
           url.contains(".youku.com/", Qt::CaseInsensitive) ||
           url.contains(".sina.com.cn/", Qt::CaseInsensitive) ||
           url.contains(".tudou.com/", Qt::CaseInsensitive) ||
           url.contains(".qq.com/", Qt::CaseInsensitive))
    ret = Traits::Chinese;
  return ret;
}

// EOF
