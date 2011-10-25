// core/gui/webdialog.cc
// 10/9/2011

#include "commentview.h"
#include "global.h"

#include <QtGui>
#include <QtWebKit>

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

Core::Gui::
WebDialog::WebDialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent)
{
  setWindowFlags(f ? f : WINDOW_FLAGS);
}

// EOF
