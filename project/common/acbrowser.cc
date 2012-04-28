// acbrowser.cc
// 4/9/2012

#include "project/common/acbrowser.h"
#include "project/common/acglobal.h"
#include "project/common/acipc.h"

#define APP_NAME        AC_BROWSER
#define APP_ID          AC_BROWSER_ID

// - Queries -

bool
AcBrowser::isRunning() const
{ return AcIpcController::isProcessRunning(APP_NAME); }

// - Actions -

void
AcBrowser::open()
{ AcIpcController::open(APP_NAME); }

void
AcBrowser::openArguments(const QStringList &args)
{ AcIpcController::open(APP_NAME, args); }

// EOF
