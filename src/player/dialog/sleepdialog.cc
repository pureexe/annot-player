// sleepdialog.cc
// 2/13/2012

#include "sleepdialog.h"
#include "tr.h"
#include "lib/qtext/htmltag.h"
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#elif defined(Q_OS_MAC)
# include "mac/qtmac/qtmac.h"
#else
# include "unix/qtunix/qtunix.h"
#endif // Q_OS_
#include <QtGui>

#define DEBUG "sleepdialog"
#include "lib/debug/debug.h"

SleepDialog::SleepDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Sleep?"));
  setText(tr("Put the computer to sleep after %1 seconds ...")
    .arg(HTML_SS_OPEN(color:red) "%1" HTML_SS_CLOSE())
  );
  QApplication::beep();
}

void
SleepDialog::run()
{
  DOUT("enter");
  QApplication::beep();
  hide();
  emit warning(tr("putting the computer to sleep ..."));
  bool ok = false;
#ifdef Q_OS_WIN
  ok = QtWin::hibernate();
#elif defined(Q_OS_MAC)
  ok = QtMac::sleep();
#else
  //QtUnix::hibernate(); // FIXME
#endif // Q_OS_
  if (!ok)
    emit warning(tr("failed to put the computer to sleep"));
  DOUT("exit");
}

// EOF
