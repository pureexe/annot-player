// logger.cc
// 10/16/2011
#include "logger.h"
#include "osdconsole.h"
#include "module/qtext/htmltag.h"
#include <QtCore>

enum { MAX_LENGTH = 75 };

void
Logger::log(const QString &message)
{
  if (message.size() < MAX_LENGTH)
    gConsole().append(
      HTML_STYLE_OPEN(color:blue) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    gConsole().append(
      HTML_STYLE_OPEN(color:blue) ": "
      + message.left(MAX_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      HTML_STYLE_CLOSE() HTML_BR()
    );
  LoggerSignals::globalInstance()->emit_logged(
    HTML_STYLE_OPEN(color:blue) ": "
    + message +
    HTML_STYLE_CLOSE()
  );
}

void
Logger::notify(const QString &message)
{
  if (message.size() < MAX_LENGTH)
    gConsole().append(
      HTML_STYLE_OPEN(color:purple) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    gConsole().append(
      HTML_STYLE_OPEN(color:purple) ": "
      + message.left(MAX_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      HTML_STYLE_CLOSE() HTML_BR()
    );
  LoggerSignals::globalInstance()->emit_notified(
    HTML_STYLE_OPEN(color:purple) ": "
    + message +
    HTML_STYLE_CLOSE()
  );
}

void
Logger::warn(const QString &message)
{
  if (message.size() < MAX_LENGTH)
    gConsole().append(
      HTML_STYLE_OPEN(color:orange)
      ": " + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    gConsole().append(
      HTML_STYLE_OPEN(color:orange) ": "
      + message.left(MAX_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      HTML_STYLE_CLOSE() HTML_BR()
    );
  LoggerSignals::globalInstance()->emit_warned(
    HTML_STYLE_OPEN(color:orange) ": "
    + message +
    HTML_STYLE_CLOSE()
  );
}

void
Logger::error(const QString &message)
{
  if (message.size() < MAX_LENGTH)
    gConsole().append(
      HTML_STYLE_OPEN(color:red) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    gConsole().append(
      HTML_STYLE_OPEN(color:red) ": "
      + message.left(MAX_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      HTML_STYLE_CLOSE() HTML_BR()
    );
  LoggerSignals::globalInstance()->emit_errored(
    HTML_STYLE_OPEN(color:red) ": "
    + message +
    HTML_STYLE_CLOSE()
  );
}

// EOF
