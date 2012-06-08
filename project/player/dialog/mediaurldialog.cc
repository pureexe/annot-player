// mediaurldialog.cc
// 2/7/2012

#include "mediaurldialog.h"
#include <QtGui>

// - Constructions -

MediaUrlDialog::MediaUrlDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Stream Media from URL"));
  //setExampleUrl(tr("http://www.youtube.com/watch?v=-DJqnomZoLk"));
  setExampleUrl(tr("http://www.nicovideo.jp/watch/1327400176"));
}

// EOF
