// aboutdialog.cc
// 11/13/2011

#include "aboutdialog.h"
#include "global.h"

AboutDialog::AboutDialog(QWidget *parent)
  : Base(G_APPLICATION, G_VERSION, parent)
{ }
