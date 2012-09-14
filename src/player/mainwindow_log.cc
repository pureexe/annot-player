// mainwindow_log.cc
// 10/16/2011
#include "mainwindow.h"
#include "mainconsole.h"
#include "miniconsole.h"
#include "consoledialog.h"
#include "lib/qtext/htmltag.h"

enum { LEFT_LENGTH = 100, RIGHT_LENGTH = 0 };

// orange: 255,165,0
void
MainWindow::showMessage(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      //HTML_SS_OPEN(color:blue)
      ": "
      + message +
      //HTML_SS_CLOSE()
      HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      //HTML_SS_OPEN(color:blue)
      ": "
      + message.left(LEFT_LENGTH) +
      HTML_SS_OPEN(color:rgba(255,165,0,200)) " ..." HTML_SS_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      //HTML_SS_CLOSE()
      HTML_BR()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      //HTML_SS_OPEN(color:blue)
      "* " +
      message +
      //HTML_SS_CLOSE()
      HTML_BR()
    );
}

void
MainWindow::showMessageOnce(const QString &message)
{
  miniConsole_->sendMessage(message);
  if (consoleDialog_)
    consoleDialog_->sendLogText("* " + message + HTML_BR());
}


// cyan: 0,255,255
void
MainWindow::notify(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) ": "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) ": "
      + message.left(LEFT_LENGTH) +
      HTML_SS_OPEN(color:rgba(255,165,0,200)) " ..." HTML_SS_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_SS_CLOSE() HTML_BR()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) "* "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}

void
MainWindow::notifyOnce(const QString &message)
{
  miniConsole_->sendMessage(
    HTML_SS_OPEN(color:rgba(0,255,255,200))
    + message +
    HTML_SS_CLOSE()
  );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) "* "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}

void
MainWindow::warn(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(0,255,255,200))
      ": " + message +
      HTML_SS_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) ": "
      + message.left(LEFT_LENGTH) +
      HTML_SS_OPEN(color:rgba(255,165,0,200)) " ..." HTML_SS_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_SS_CLOSE() HTML_BR()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) "* "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}

void
MainWindow::warnOnce(const QString &message)
{
  miniConsole_->sendMessage(
    HTML_SS_OPEN(color:rgba(0,255,255,200))
    + message +
    HTML_SS_CLOSE()
  );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(0,255,255,200)) "* "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}


void
MainWindow::showError(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(255,0,0,200)) ": "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_SS_OPEN(color:rgba(255,0,0,200)) ": "
      + message.left(LEFT_LENGTH) +
      HTML_SS_OPEN(color:rgba(255,165,0,200)) " ..." HTML_SS_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_SS_CLOSE() HTML_BR()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(255,0,0,200)) ": "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}

void
MainWindow::showErrorOnce(const QString &message)
{
  miniConsole_->sendMessage(
    HTML_SS_OPEN(color:rgba(255,0,0,200))
    + message +
    HTML_SS_CLOSE()
  );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_SS_OPEN(color:rgba(255,0,0,200)) "* "
      + message +
      HTML_SS_CLOSE() HTML_BR()
    );
}

// EOF
