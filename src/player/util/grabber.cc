// grabber.cc
// 10/10/2011

#include "grabber.h"
#include "lib/qtext/filesystem.h"
#include <QtGui>

// - Constructions -

Grabber::Grabber(QObject *parent)
  : Base(parent)
{
  savePath_ = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  baseName_ = tr("Unknown");
}

// - Properties -

void
Grabber::setBaseName(const QString &name)
{
  if (name.isEmpty())
    baseName_ = tr("Unknown");
  else
    baseName_ = QtExt::escapeFileName(name);
}

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
  bool ok = pm.save(file, "png");

  if (ok)
    emit message(tr("file saved") + ": " + file);
  else
    emit warning(tr("failed to save file") + ": " + file);
}

// EOF
