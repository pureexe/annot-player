// translatebrowser.cc
// 8/13/2012

#include "translatebrowser.h"
#include "project/common/acss.h"

//#define DEBUG "translatebrowser"
#include "module/debug/debug.h"

// - Constructions -

TranslateBrowser::TranslateBrowser(QWidget *parent)
  : Base(parent)
{
  setStyleSheet(ACSS_TEXTBROWSER);
  setReadOnly(true);
  setOpenLinks(false);

  connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText()));
}

// - Actions -

void
TranslateBrowser::updateSelectedText()
{
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t)
    emit selectedTextChanged(selectedText_ = t);
}

// EOF
