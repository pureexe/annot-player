// httpsession_p.cc
// 2/21/2012

#include "lib/vlchttp/httpsession_p.h"

//#define DEBUG "httpsession"
#include "lib/debug/debug.h"

bool
VlcHttpSession::isMultiMediaMimeType(const QString &contentType)
{ return !contentType.isEmpty() && !contentType.startsWith("text/", Qt::CaseInsensitive); }

// EOF
