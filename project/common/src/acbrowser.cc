// acbrowser.cc
// 4/9/2012

#include "ac/acbrowser.h"
#include "ac/acglobal.h"

#define APP_NAME        AC_BROWSER
#define APP_ID          AC_BROWSER_ID

// - Construction -

AcBrowserController::AcBrowserController(QObject *parent, AcIpcController::Role role)
  : Base(parent)
{ Q_UNUSED(role); }

// - Queries -

bool
AcBrowserController::isRunning() const
{ return AcIpcController::isProcessRunning(APP_NAME); }

// - Actions -

void
AcBrowserController::open()
{ AcIpcController::open(APP_NAME); }

void
AcBrowserController::openUrl(const QString &url)
{ openUrls(QStringList(url)); }

void
AcBrowserController::openUrls(const QStringList &urls)
{ AcIpcController::open(APP_NAME, urls); }

// EOF
/*
// - Construction -

AcBrowserController::AcBrowserController(QObject *parent, Role role)
  : Base(role ? role : APP_ROLE, parent, APP_ID)
{ setEnabled(false); }
*/
