// qtext/filedeleter.cc
// 2/6/2012

#include "module/qtext/filedeleter.h"
#include <QtCore>

// - Constructions -

FileDeleter::~FileDeleter()
{ deleteAll(); }

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
  foreach (QString path, files_)
    QFile::remove(path);
}

// EOF
