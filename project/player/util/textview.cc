// textview.cc
// 8/12/2012

#include "textview.h"
#include <QtGui/QTextCursor>

// - Construction -

TextView::TextView(QWidget *parent)
  : Base(parent), selectionMonitored_(false)
{ connect(this, SIGNAL(selectionChanged()), SLOT(updateSelectedText())); }

// - Actions -

void
TextView::updateSelectedText()
{
  if (!selectionMonitored_)
    return;
  QString t = textCursor().selectedText().trimmed();
  if (selectedText_ != t)
    emit selectedTextChanged(selectedText_ = t);
}

// EOF
