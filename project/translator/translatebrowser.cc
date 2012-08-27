// translatebrowser.cc
// 8/13/2012

#include "translatebrowser.h"
#include "project/common/acss.h"
#include "project/common/acrc.h"
#include "module/qtext/texthighlighter.h"
#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

//#define DEBUG "translatebrowser"
#include "module/debug/debug.h"

// - Constructions -

TranslateBrowser::TranslateBrowser(QWidget *parent)
  : Base(parent)
{
  setStyleSheet(ACSS_TEXTBROWSER);
  setReadOnly(true);
  setOpenLinks(false);

  highlighter_ = new QtExt::TextHighlighter(this);

  translateAct_ = new QAction(tr("Translate"), this);
  connect(translateAct_, SIGNAL(triggered()), SLOT(invalidateSelectedText()));
  //translateAct_->setShortcut(QKeySequence("CTRL+S"));
  //addAction(translateAct_);

  clearAct_ = new QAction(tr("Clear All"), this);
  connect(clearAct_, SIGNAL(triggered()), SLOT(clear()));

  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
TranslateBrowser::highlightText(const QString &t)
{ highlighter_->setHighlightText(t); }

void
TranslateBrowser::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t)
    emit selectedTextChanged(selectedText_ = t);
}

QVariant
TranslateBrowser::loadResource(int type, const QUrl &name)
{
  if (type == QTextDocument::ImageResource)
    return QImage(ACRC_IMAGE_NULL);
  return Base::loadResource(type, name);
}

// - Events -

void
TranslateBrowser::contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *m = createStandardContextMenu();
  m->addSeparator();
  m->addAction(clearAct_);
  m->addAction(translateAct_);
  m->exec(e->globalPos());
  e->accept();
}

// EOF
