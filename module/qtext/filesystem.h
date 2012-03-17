#ifndef _QTEXT_FILESYSTEM_H
#define _QTEXT_FILESYSTEM_H

// qtext/filesystem.h
// 3/16/2012

#include <QString>

namespace QtExt {

  inline QString escapeFileName(const QString &name)
  {
    QString ret = name;
    ret.remove('"');
    ret.remove('\'');
    ret.replace('/', "／");
    ret.replace('\\', "-");
    ret.replace('|', "-");
    ret.replace(':', "-");
    ret.replace('?', "？");
    return ret.trimmed();
  }

} // namespace QtExt

#endif // _QTEXT_FILESYSTEM_H
