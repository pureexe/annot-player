// annoturldialog.cc
// 2/7/2012

#include "annoturldialog.h"

// - Constructions -

AnnotUrlDialog::AnnotUrlDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Import Annotations from URL"));
  setExampleUrl("http://www.nicovideo.jp/watch/1284843355");
}

// EOF
