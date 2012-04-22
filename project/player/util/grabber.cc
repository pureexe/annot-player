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

const QString&
Grabber::savePath() const
{ return savePath_; }

void
Grabber::setBaseName(const QString &name)
{ baseName_ = name; }

void
Grabber::setSavePath(const QString &path)
{ savePath_ = path; }

// - Actions -

void
Grabber::grabDesktop()
{ grabWindow(QApplication::desktop()->winId());  }

void
Grabber::grabWindow(WId winId)
{
  QPixmap pm = QPixmap::grabWindow(winId);
  QClipboard *c = QApplication::clipboard();
  if (c)
    c->setPixmap(pm);

  QString ts = QDateTime::currentDateTime().toString("-yyyy-mm-dd-hh-mm-ss");
  QString file = QString("%1/%2%3.png").arg(savePath_).arg(baseName_).arg(ts);
  pm.save(file, "png");
}

// EOF
