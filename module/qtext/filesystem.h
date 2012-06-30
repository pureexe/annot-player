#ifndef _QTEXT_FILESYSTEM_H
#define _QTEXT_FILESYSTEM_H

// qtext/filesystem.h
// 3/16/2012

#include <QtCore/QString>

#ifdef Q_WS_WIN
# define FILE_PATH_SEP      "\\"
#else
# define FILE_PATH_SEP      "/"
#endif // Q_WS_WIN

namespace QtExt {

  QString escapeFileName(const QString &name);

  bool touchFile(const QString &fileName);

} // namespace QtExt

#endif // _QTEXT_FILESYSTEM_H
