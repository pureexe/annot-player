// qtext/webdialog.cc
// 10/9/2011

#include "module/qtext/webdialog.h"
#include <QtGui>
#include <QtWebKit>

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

QtExt::
WebDialog::WebDialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent)
{
  setWindowFlags(f ? f : WINDOW_FLAGS);
}

// EOF
