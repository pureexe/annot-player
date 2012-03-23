// annotationthreadview.cc
// 11/16/2011

#include "annotationthreadview.h"
#include "defines.h"
#include "tr.h"
#include "module/annotcloud/annothtml.h"
#include <QtGui>
#include <QtWebKit>

#define DEBUG "annotationthreadview"
#include "module/debug/debug.h"

#define WINDOW_SIZE     QSize(640, 640)
#define BASE_URL        G_HOMEPAGE "/api/player/"
#define EMPTY_URL       BASE_URL "empty.html"

using namespace AnnotCloud;

#define TEXT_SIZE_SCALE 0.85

// - Constructions -
AnnotationThreadView::AnnotationThreadView(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(TR(T_TITLE_ANNOTTHREAD));
  setTextSizeMultiplier(TEXT_SIZE_SCALE);
  resize(WINDOW_SIZE);

  setupActions();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
}

void
AnnotationThreadView::setupActions()
{
  QAction *a = pageAction(QWebPage::Reload);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));

  a = pageAction(QWebPage::ReloadAndBypassCache);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));
}

// - Actions -

void
AnnotationThreadView::refresh()
{
  DOUT("enter");
  emit annotationsRequested();
  if (annots_.isEmpty())
    load(QUrl(EMPTY_URL));
  else {
    QString title = TR(T_TITLE_ANNOTTHREAD) +
        QString(" (%1)").arg(QString::number(annots_.size()));
    setWindowTitle(title);

    QString html = AnnotationHtmlParser::globalInstance()->toHtml(annots_);
    QString mimeType;
    QString baseUrl = BASE_URL;
    setContent(html.toUtf8(), mimeType, baseUrl);
  }
  DOUT("exit");
}

// - Events -

void
AnnotationThreadView::setVisible(bool visible)
{
  if (visible)
    refresh();
  Base::setVisible(visible);
}

// EOF
