// qxfiledeleter.cc
// 2/6/2012

#include "qtx/qxfiledeleter.h"
#include <QtCore/QFile>

QTX_BEGIN_NAMESPACE

// - Deletion -

void
QxFileDeleter::deleteFileLater(const QString &path, bool async)
{
  if (async)
    mutex_.lock();
  if (!files_.contains(path))
    files_.append(path);
  if (async)
    mutex_.unlock();
}

void
QxFileDeleter::deleteAll()
{
  foreach (const QString &path, files_)
    QFile::remove(path);
}

QTX_END_NAMESPACE
