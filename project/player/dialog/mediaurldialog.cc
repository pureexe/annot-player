// mediaurldialog.cc
// 2/7/2012

#include "mediaurldialog.h"
#include <QtGui>

// - Constructions -

MediaUrlDialog::MediaUrlDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Open media from URL"));
  setExampleUrl(tr("http://www.youtube.com/watch?v=-DJqnomZoLk"));
}

// EOF
