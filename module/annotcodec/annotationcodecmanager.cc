// annotationcodecmanager.cc
// 2/4/2012
#include "annotationcodecmanager.h"
#include "bilibilicodec.h"
#include "acfuncodec.h"
#include "nicovideocodec.h"
#include <QtCore>

//#define DEBUG "annotationcodecmanager"
#include "module/debug/debug.h"

using namespace AnnotCloud;

// - Construction -

AnnotationCodecManager::AnnotationCodecManager(QObject *parent)
  : Base(parent)
{
  AnnotationCodec *c;
#define ADD(_codec) \
  c = new _codec(this); { \
    connect(c, SIGNAL(errorReceived(QString)), SIGNAL(errorReceived(QString))); \
    connect(c, SIGNAL(messageReceived(QString)), SIGNAL(messageReceived(QString))); \
    connect(c, SIGNAL(fetched(AnnotationList,QString)), SIGNAL(fetched(AnnotationList,QString))); \
  } codecs_.append(c);

  ADD(BilibiliCodec)
  ADD(AcFunCodec)
  ADD(NicovideoCodec)
#undef ADD
}

// - Analysis -

int
AnnotationCodecManager::match(const QString &url) const
{
  for (int i = 0; i < codecs_.size(); i++)
    if (codecs_[i]->match(url))
      return i;
  return -1;
}

void
AnnotationCodecManager::fetch(int id, const QString &url)
{
  if (id < 0 || id > codecs_.size()) {
    Q_ASSERT(0);
    DOUT("illegal state: codec id out of bounds");
    return;
  }
  codecs_[id]->fetch(url);
}

// EOF
