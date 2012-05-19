// clipboardmonitor.cc
// 3/10/2012

#include "clipboardmonitor.h"
#include "logger.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtGui>

using namespace Logger;

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
    log(tr("annot URL from clipboard") + ": " + url);
    emit annotationUrlEntered(url);
  } else if (isSupportedMediaUrl(url)) {
    log(tr("media URL from clipboard") + ": " + url);
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
