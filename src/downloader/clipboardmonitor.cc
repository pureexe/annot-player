// clipboardmonitor.cc
// 3/10/2012

#include "clipboardmonitor.h"
#include "lib/mrlanalysis/mrlanalysis.h"
#include <QtGui>

#define DEBUG "clipboardmonitor"
#include "qtx/qxdebug.h"

// - Constructions -

ClipboardMonitor::ClipboardMonitor(QObject *parent)
  : Base(parent), enabled_(true)
{
  QClipboard *c = QApplication::clipboard();
  if (c)
    connect(c, SIGNAL(dataChanged()), SLOT(invalidateClipboard()));
}

// - Slots -

void
ClipboardMonitor::invalidateClipboard()
{
  if (!isEnabled())
    return;

  QClipboard *c = QApplication::clipboard();
  if (!c)
    return;
  QString text = c->text().trimmed();
  if (!text.isEmpty())
    parseUrl(text);
}

void
ClipboardMonitor::parseUrl(const QString &text)
{
  QStringList l = text.split('\n', QString::SkipEmptyParts);
  foreach (QString url, l) {
    url = url.trimmed();
    if (url.startsWith("ttp://"))
      url.prepend("h");
    else if (!url.startsWith("http://", Qt::CaseInsensitive))
      url.prepend("http://");
    if (isSupportedMediaUrl(url))
      emit urlEntered(url);
  }
}

// - Helpers -

bool
ClipboardMonitor::isSupportedMediaUrl(const QString &url)
{
  int site;
  return (site = MrlAnalysis::matchSite(url)) &&
          site < MrlAnalysis::ChineseVideoSite; // TODO: change to all sites after fixing youtube
}

// EOF
