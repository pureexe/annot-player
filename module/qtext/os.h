#ifndef _QTEXT_OS_H
#define _QTEXT_OS_H

// qtext/os.h
// 4/6/2012

#include <QString>

namespace QtExt {

  void sleep(uint msecs);

  QString mktemp(const QString &suffix = QString(), bool deleteLater = true);

} // namespace QtExt

#endif // _QTEXT_OS
