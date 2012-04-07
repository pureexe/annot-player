// clipboardmonitor.cc
// 3/10/2012

#include "clipboardmonitor.h"
#include "logger.h"
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
  if (t.startsWith("ttp://", Qt::CaseInsensitive))
    t.prepend("h");
  else if (!t.startsWith("http://", Qt::CaseInsensitive))
    return;

  QStringList l = t.split('\n', QString::SkipEmptyParts);
  Q_ASSERT(!l.isEmpty());
  QString url = l.front().trimmed();
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
  return url.startsWith("http://www.nicovideo.jp/", Qt::CaseInsensitive) ||
         url.startsWith("http://nicovideo.jp/", Qt::CaseInsensitive) ||
         url.startsWith("http://www.bilibili.tv/", Qt::CaseInsensitive) ||
         url.startsWith("http://bilibili.tv/", Qt::CaseInsensitive) ||
         url.startsWith("http://www.acfun.tv/", Qt::CaseInsensitive) ||
         url.startsWith("http://acfun.tv/", Qt::CaseInsensitive);
}

bool
ClipboardMonitor::isSupportedMediaUrl(const QString &url)
{
  return //isSupportedAnnotationUrl(url) ||
         url.startsWith("http://www.youtube.com", Qt::CaseInsensitive) ||
         url.startsWith("http://youtube.com", Qt::CaseInsensitive) ||
         url.startsWith("http://v.youku.com/", Qt::CaseInsensitive) ||
         url.startsWith("http://video.sina.com.cn", Qt::CaseInsensitive);
}

// EOF
