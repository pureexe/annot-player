// annotdownurldialog.cc
// 7/20/2012

#include "annotdownurldialog.h"

// - Constructions -

AnnotDownUrlDialog::AnnotDownUrlDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Download XML/JSON Annotations"));
  setExampleUrl("http://www.nicovideo.jp/watch/1284843355");
}

// EOF
