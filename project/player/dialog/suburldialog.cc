// suburldialog.cc
// 2/7/2012

#include "suburldialog.h"
#include <QtGui>

// - Constructions -

SubUrlDialog::SubUrlDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Import Annotations from URL"));
  setExampleUrl("http://www.nicovideo.jp/watch/1284843355");
}

// EOF
