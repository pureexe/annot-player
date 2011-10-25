// grabber.cc
// 10/10/2011

#include "grabber.h"
#include <QtGui>

// - Constructions -

Grabber::Grabber(QObject *parent)
  : Base(parent)
{
  savePath_ = QDir::homePath();
  baseName_ = tr("unknown");
}

const QString&
Grabber::baseName() const
{ return baseName_; }

void
Grabber::setBaseName(const QString &name)
{ baseName_ = name; }

// - Actions -

void
Grabber::grabDesktop()
{ grabWindow(QApplication::desktop()->winId());  }

void
Grabber::grabWindow(WId winid)
{
  QPixmap pm = QPixmap::grabWindow(QApplication::desktop()->winId());

  QString ts = QDateTime::currentDateTime().toString("-yyyy-mm-dd-hh-mm-ss");
  QString file = QString("%1/%2%3.png").arg(savePath_).arg(baseName_).arg(ts);
  pm.save(file, "PNG");
}

// EOF
