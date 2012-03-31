// sleepdialog.cc
// 2/13/2012

#include "sleepdialog.h"
#include "tr.h"
#include "logger.h"
#include "module/qtext/htmltag.h"
#ifdef Q_OS_WIN
#  include "win/qtwin/qtwin.h"
#elif defined Q_OS_MAC
#  include "mac/qtmac/qtmac.h"
#else
#  include "unix/qtunix/qtunix.h"
#endif // Q_OS_
#include <QtGui>

using namespace Logger;

#define DEBUG "sleepdialog"
#include "module/debug/debug.h"

SleepDialog::SleepDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Sleep?"));
  setText(tr("Put the computer to sleep after %1 seconds ...")
    .arg(HTML_STYLE_OPEN(color:red) "%1" HTML_STYLE_CLOSE())
  );
  QApplication::beep();
}

void
SleepDialog::run()
{
  DOUT("enter");
  QApplication::beep();
  hide();
  warn(tr("putting the computer to sleep ..."));
#ifdef Q_OS_WIN
  QtWin::hibernate();
#elif defined Q_OS_MAC
  QtMac::sleep();
#else
  //QtUnix::hibernate(); // FIXME
#endif // Q_OS_
  DOUT("exit");
}

// EOF
