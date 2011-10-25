// logger.cc
// 10/16/2011
#include "logger.h"
#include "osdconsole.h"
#include "core/htmltag.h"
#include <QtCore>

void
Logger::log(const QString &message)
{
  gConsole() << CORE_HTML_STYLE_OPEN(color:blue)
               ": " << message
             << CORE_HTML_STYLE_CLOSE() CORE_HTML_BR();
}

void
Logger::notify(const QString &message)
{
  gConsole() << CORE_HTML_STYLE_OPEN(color:orange)
               ": " << message
             << CORE_HTML_STYLE_CLOSE() CORE_HTML_BR();
}

void
Logger::warn(const QString &message)
{
  gConsole() << CORE_HTML_STYLE_OPEN(color:purple)
               ": " << message
             << CORE_HTML_STYLE_CLOSE() CORE_HTML_BR();
}

void
Logger::error(const QString &message)
{
  gConsole() << CORE_HTML_STYLE_OPEN(color:red)
               ": " << message
             << CORE_HTML_STYLE_CLOSE() CORE_HTML_BR();
}

// EOF
