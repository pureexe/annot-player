// logger.cc
// 10/16/2011
#include "logger.h"
#include "osdconsole.h"
#include "module/qtext/htmltag.h"
#include <QtCore>

void
Logger::log(const QString &message)
{
  gConsole().append(
    HTML_STYLE_OPEN(color:blue)
    ": " + message +
    HTML_STYLE_CLOSE() HTML_BR()
  );
}

void
Logger::notify(const QString &message)
{
  gConsole().append(
    HTML_STYLE_OPEN(color:purple)
    ": " + message +
    HTML_STYLE_CLOSE() HTML_BR()
  );
}

void
Logger::warn(const QString &message)
{
  gConsole().append(
    HTML_STYLE_OPEN(color:orange)
    ": " + message +
    HTML_STYLE_CLOSE() HTML_BR()
  );
}

void
Logger::error(const QString &message)
{
  gConsole().append(
    HTML_STYLE_OPEN(color:red)
    ": " + message +
    HTML_STYLE_CLOSE() HTML_BR()
  );
}

// EOF
