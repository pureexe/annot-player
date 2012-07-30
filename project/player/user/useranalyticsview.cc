// useranalyticsview.cc
// 5/7/2012

#include "useranalyticsview.h"
#include "global.h"
#include "tr.h"
#include "datamanager.h"
#include "signalhub.h"
#include "module/annotcloud/annothtml.h"
#ifdef Q_OS_MAC
# include "mac/qtmac/qtmac.h"
#endif // Q_OS_MAC
#include <QtGui>
#include <QtWebKit>

#define DEBUG "annotationanalyticsview"
#include "module/debug/debug.h"

#define WINDOW_SIZE     QSize(640, 640)
#define BASE_URL        G_HOMEPAGE "/clients/player/"
#define EMPTY_URL       BASE_URL "empty.html"

using namespace AnnotCloud;

// - Construction -

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

UserAnalyticsView::UserAnalyticsView(DataManager *data, SignalHub *hub, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), data_(data), hub_(hub), userId_(0)
{
  Q_ASSERT(data_);
  setWindowTitle(tr("User Analytics"));
  resize(WINDOW_SIZE);

  setCacheDirectory(G_PATH_CACHES);
  webView()->setRenderHints(
    QPainter::Antialiasing |
    QPainter::TextAntialiasing |
    //QPainter::HighQualityAntialiasing |
    QPainter::SmoothPixmapTransform
  );

  setupActions();

  // Shortcuts
  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
}

void
UserAnalyticsView::setupActions()
{
  QAction *a = webView()->pageAction(QWebPage::Reload);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));

  a = webView()->pageAction(QWebPage::ReloadAndBypassCache);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));
}

// - Properties -

AnnotationList
UserAnalyticsView::userAnnotations() const
{
  AnnotationList ret;
  if (userId_)
    foreach (const Annotation &a, data_->annotations())
      if (a.userId() == userId_)
        ret.append(a);
  return ret;
}

// - Actions -

void
UserAnalyticsView::refresh()
{
  DOUT("enter: userId =" << userId_);
  invalidateAnnotations();
  DOUT("exit");
}

void
UserAnalyticsView::setUrl(const QUrl &url)
{ webView()->load(url); }

void
UserAnalyticsView::setContent(const QString &html)
{
  QString mimeType,
          baseUrl = BASE_URL;
  webView()->setContent(html.toUtf8(), mimeType, baseUrl);
}

void
UserAnalyticsView::invalidateAnnotations()
{
  QString title = tr("User");
  AnnotationList a = userAnnotations();
  if (a.isEmpty())
    setUrl(QUrl(EMPTY_URL));
  else {
    title.append(QString(" (%1)").arg(QString::number(userId_, 16)));
    bool ignorePos = !hub_->isMediaTokenMode();
    QString html = AnnotationHtmlParser::globalInstance()->toHtml(data_->annotations(), title, ignorePos);
    setContent(html);
  }

  setWindowTitle(title);
}

void
UserAnalyticsView::setVisible(bool visible)
{
  if (visible) {
    refresh();
    if (isVisible())
      raise();
  }
  Base::setVisible(visible);
}

// EOF
