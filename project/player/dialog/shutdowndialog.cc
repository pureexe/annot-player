// shutdowndialog.cc
// 2/13/2012

#include "shutdowndialog.h"
#include "tr.h"
#include "logger.h"
#include "module/qtext/htmltag.h"
#ifdef Q_OS_WIN
  #include "win/qtwin/qtwin.h"
#elif defined Q_OS_MAC
  #include "mac/qtmac/qtmac.h"
#else
  #include "unix/qtunix/qtunix.h"
#endif // Q_OS_
#include <QtGui>

using namespace Logger;

#define DEBUG "shutdowndialog"
#include "module/debug/debug.h"

ShutdownDialog::ShutdownDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Shutdown?"));
  setText(tr("Shutdown the computer after %1 seconds ...")
    .arg(HTML_STYLE_OPEN(color:red) "%1" HTML_STYLE_CLOSE())
  );
  QApplication::beep();
}

void
ShutdownDialog::run()
{
  DOUT("enter");
  QApplication::beep();
  hide();
  warn(tr("shutting down the computer ..."));
#ifdef Q_OS_WIN
  QtWin::halt();
#elif defined Q_OS_MAC
  QtMac::halt();
#else
  QtUnix::halt();
#endif // Q_OS_
  DOUT("exit");
}

// EOF
