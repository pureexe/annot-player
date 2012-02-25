// mediastreamer.cc
// 2/16/2016

#include "mediastreamer.h"
#include "mediastreamtask.h"
#include <QtCore>

// - Task -

void
MediaStreamer::stop()
{
  if (t_) {
    t_->stop();
    delete t_;
  }
}

// - Stream -

void
MediaStreamer::stream(const MediaStreamInfo &msi, QNetworkAccessManager *nam, bool async)
{
  stop();
  if (msi.urls.isEmpty())
    return;
  t_ = new MediaStreamTask(msi, nam);
  t_->setAutoDelete(false);
  if (async)
    QThreadPool::globalInstance()->start(t_);
  else
    t_->run();
}

// EOF
