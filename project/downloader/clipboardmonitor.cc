// clipboardmonitor.cc
// 3/10/2012

#include "clipboardmonitor.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtGui>

#define DEBUG "clipboardmonitor"
#include "module/debug/debug.h"

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
  QString text = c->text();
  if (!text.isEmpty())
    parseUrl(text);
}

void
ClipboardMonitor::parseUrl(const QString &text)
{
  QString t = text.trimmed();
  if (t.isEmpty())
    return;
  QStringList l = t.split('\n', QString::SkipEmptyParts);
  Q_ASSERT(!l.isEmpty());
  QString url = l.front().trimmed();
  if (url.startsWith("ttp://"))
    url.prepend("h");
  else if (!url.startsWith("http://", Qt::CaseInsensitive))
    url.prepend("http://");
  if (isSupportedAnnotationUrl(url)) {
    emit message(tr("annot URL from clipboard") + ": " + url);
    emit annotationUrlEntered(url);
  } else if (isSupportedMediaUrl(url)) {
    emit message(tr("media URL from clipboard") + ": " + url);
    emit mediaUrlEntered(url);
  } else
    DOUT("unsupported URL:" << url);
}

// - Helpers -

bool
ClipboardMonitor::isSupportedAnnotationUrl(const QString &url)
{
  MrlAnalysis::Site site;
  return (site = MrlAnalysis::matchSite(url)) &&
          site < MrlAnalysis::AnnotationSite;
}

bool
ClipboardMonitor::isSupportedMediaUrl(const QString &url)
{
  MrlAnalysis::Site site;
  return (site = MrlAnalysis::matchSite(url)) &&
          site < MrlAnalysis::ChineseVideoSite; // TODO: change to all sites after fixing youtube
}

// EOF
