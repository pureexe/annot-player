// qtext/filedeleter.cc
// 2/6/2012

#include "lib/qtext/filedeleter.h"
#include <QtCore/QFile>

// - Deletion -

void
FileDeleter::deleteFileLater(const QString &path, bool async)
{
  if (async)
    mutex_.lock();
  if (!files_.contains(path))
    files_.append(path);
  if (async)
    mutex_.unlock();
}

void
FileDeleter::deleteAll()
{
  foreach (const QString &path, files_)
    QFile::remove(path);
}

// EOF
