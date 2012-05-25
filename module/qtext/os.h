#ifndef _QTEXT_OS_H
#define _QTEXT_OS_H

// qtext/os.h
// 4/6/2012

#include <QtCore/QString>

namespace QtExt {

  QString mktemp(const QString &suffix = QString(), bool deleteLater = true);

  void sleep(uint msecs);

  class SleepTimer
  {
     bool stopped_;
  public:
    void stop() { stopped_ = true; }

    void start(int secs)
    {
      stopped_ = false;
      while (secs-- > 0 && !stopped_)
        sleep(1000);
    }
  };

} // namespace QtExt

#endif // _QTEXT_OS
