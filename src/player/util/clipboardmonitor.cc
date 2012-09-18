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
ClipboardMonitor::setEnabled(bool t)
{
  enabled_ = t;
  if (enabled_)
    QTimer::singleShot(0, this, SLOT(invalidateClipboard()));
}

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
  if (l.isEmpty())
    return;
  QString url = l.first().trimmed();
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
  int site;
  return (site = MrlAnalysis::matchSite(url)) &&
          site < MrlAnalysis::AnnotationSite;
}

bool
ClipboardMonitor::isSupportedMediaUrl(const QString &url)
{
  int site;
  return (site = MrlAnalysis::matchSite(url)) &&
          site < MrlAnalysis::VideoSite;
}

// EOF
