// acupdater.cc
// 8/16/2012

#include "src/common/acupdater.h"
#include "src/common/acglobal.h"
#include "src/common/acipc.h"

#define APP_NAME        AC_UPDATER
#define APP_ID          AC_UPDATER_ID

#ifdef Q_OS_WIN
# define APP_PATH       "../../Update.exe"
#else
# define APP_PATH       APP_NAME
#endif // Q_OS_WIN

// - Queries -

bool
AcUpdater::isRunning() const
{ return AcIpcController::isProcessRunning(APP_NAME); }

// - Actions -

void
AcUpdater::open()
{ AcIpcController::open(APP_PATH); }

void
AcUpdater::openArguments(const QStringList &args)
{ AcIpcController::open(APP_PATH, args); }

// EOF
