#ifndef DOWNLOAD_H
#define DOWNLOAD_H

// download.h
// 2/4/2012

#include <QString>

bool download(const QString &url, const QString &path, bool zipped = false,  bool async = true);

#endif // DOWNLOAD
