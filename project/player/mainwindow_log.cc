// mainwindow_log.cc
// 10/16/2011
#include "mainwindow.h"
#include "mainconsole.h"
#include "miniconsole.h"
#include "consoledialog.h"
#include "module/qtext/htmltag.h"

enum { LEFT_LENGTH = 100, RIGHT_LENGTH = 0 };

void
MainWindow::showMessage(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      //HTML_STYLE_OPEN(color:blue)
      ": "
      + message +
      //HTML_STYLE_CLOSE()
      HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      //HTML_STYLE_OPEN(color:blue)
      ": "
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      //HTML_STYLE_CLOSE()
      HTML_BR()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      //HTML_STYLE_OPEN(color:blue)
      "* " +
      message +
      //HTML_STYLE_CLOSE()
      HTML_BR()
    );
}

void
MainWindow::showMessageOnce(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    miniConsole_->sendMessage(
      //HTML_STYLE_OPEN(color:blue)
      message
      //HTML_STYLE_CLOSE()
    );
  else
    miniConsole_->sendMessage(
      //HTML_STYLE_OPEN(color:blue)
      message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      //HTML_STYLE_CLOSE()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      //HTML_STYLE_OPEN(color:blue)
      "* " +
      message +
      //HTML_STYLE_CLOSE()
      HTML_BR()
    );
}


void
MainWindow::notify(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan) ": "
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE() HTML_BR()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:cyan) "* "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}

void
MainWindow::notifyOnce(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan)
      + message +
      HTML_STYLE_CLOSE()
    );
  else
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan)
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE()
    );

  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:cyan) "* "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}

void
MainWindow::warn(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan)
      ": " + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan) ": "
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:cyan) "* "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}

void
MainWindow::warnOnce(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan)
      + message +
      HTML_STYLE_CLOSE()
    );
  else
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:cyan)
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:cyan) "* "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}


void
MainWindow::showError(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:red) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  else
    mainConsole_->sendMessage(
      HTML_STYLE_OPEN(color:red) ": "
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE() HTML_BR()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:red) ": "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}

void
MainWindow::showErrorOnce(const QString &message)
{
  if (message.size() < LEFT_LENGTH + RIGHT_LENGTH)
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:red)
      + message +
      HTML_STYLE_CLOSE()
    );
  else
    miniConsole_->sendMessage(
      HTML_STYLE_OPEN(color:red)
      + message.left(LEFT_LENGTH) +
      HTML_STYLE_OPEN(color:orange) " ..." HTML_STYLE_CLOSE()
      //+ message.right(RIGHT_LENGTH) +
      HTML_STYLE_CLOSE()
    );
  if (consoleDialog_)
    consoleDialog_->sendLogText(
      HTML_STYLE_OPEN(color:red) "* "
      + message +
      HTML_STYLE_CLOSE() HTML_BR()
    );
}

// EOF

