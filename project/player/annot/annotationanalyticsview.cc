// annotationanalyticsview.cc
// 11/16/2011

#include "annotationanalyticsview.h"
#include "global.h"
#include "tr.h"
#include "datamanager.h"
#include "logger.h"
#include "module/annotcloud/annothtml.h"
#include <QtGui>
#include <QtWebKit>

using namespace Logger;

#define DEBUG "annotationanalyticsview"
#include "module/debug/debug.h"

#define WINDOW_SIZE     QSize(640, 640)
#define BASE_URL        G_HOMEPAGE "/api/player/"
#define EMPTY_URL       BASE_URL "empty.html"

using namespace AnnotCloud;

// - Task -

namespace { namespace task_ {

  class InvalidateAnnotations : public QRunnable
  {
    AnnotationAnalyticsView *w_;
    virtual void run() ///< \override
    { w_->invalidateAnnotations(false);} // async = false

  public:
    explicit InvalidateAnnotations(AnnotationAnalyticsView *w)
      : w_(w) { Q_ASSERT(w_); }
  };

} } // anonymous namespace task_


// - Construction -

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

AnnotationAnalyticsView::AnnotationAnalyticsView(DataManager *data, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), data_(data), refreshing_(false)
{
  Q_ASSERT(data_);
  setWindowTitle(TR(T_TITLE_ANNOTANALYTICS));
  resize(WINDOW_SIZE);

  setupActions();

  connect(this, SIGNAL(windowTitleChanged(QString)), SLOT(setWindowTitle(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(contentChanged(QString)), SLOT(setContent(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(urlChanged(QUrl)), SLOT(setUrl(QUrl)), Qt::QueuedConnection);
  connect(this, SIGNAL(visibleChanged(bool)), SLOT(setVisible(bool)), Qt::QueuedConnection);

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence("CTRL+W"), this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
}

void
AnnotationAnalyticsView::setupActions()
{
  QAction *a = webView()->pageAction(QWebPage::Reload);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));

  a = webView()->pageAction(QWebPage::ReloadAndBypassCache);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));
}

// - Actions -

void
AnnotationAnalyticsView::refresh()
{
  enum { AsyncLimit = 500 };

  DOUT("enter: refreshing =" << refreshing_);
  log("analyzing annotations ...");
  if (!refreshing_) {
    refreshing_ = true;
    setWindowTitle(tr("Refreshing ..."));
    bool async = data_->annotations().size() > AsyncLimit;
    invalidateAnnotations(async);
  }
  DOUT("exit");
}

void
AnnotationAnalyticsView::setUrl(const QUrl &url)
{ webView()->load(url); }

void
AnnotationAnalyticsView::setContent(const QString &html)
{
  QString mimeType,
          baseUrl = BASE_URL;
  webView()->setContent(html.toUtf8(), mimeType, baseUrl);
}

void
AnnotationAnalyticsView::invalidateAnnotations(bool async)
{
  DOUT("enter: async =" << async << ", refreshing =" << refreshing_);
  refreshing_ = true;
  if (async) {
    QThreadPool::globalInstance()->start(new task_::InvalidateAnnotations(this));
    DOUT("exit: async branch");
    return;
  }

  QString title = TR(T_TITLE_ANNOTANALYTICS);
  if (data_->hasAnnotations()) {
    title.append(QString(" (%1)").arg(QString::number(data_->annotations().size())));

    QString html = AnnotationHtmlParser::globalInstance()->toHtml(data_->annotations());
    emit contentChanged(html);
  } else
    emit urlChanged(QUrl(EMPTY_URL));

  emit windowTitleChanged(title);
  emit visibleChanged(true);
  refreshing_ = false;
  DOUT("exit");
}

// EOF
/*

void
AnnotationAnalyticsView::setVisible(bool visible)
{
  if (visible)
    refresh();
  Base::setVisible(visible);
}

*/
