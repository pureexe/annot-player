// textedittabview.cc
// 1/1/2012

#include "textedittabview.h"
#include "project/common/actextview.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/toolbuttonwithid.h"
#include <QtGui>

// - Constructions -

void
TextEditTabView::addTab(const QString &tabName)
{
  QWidget *tab = new AcTextView;
  tab->setWindowTitle(tabName);
  Base::addTab(tab);
}

AcTextView*
TextEditTabView::textWidget(int tab) const
{ return dynamic_cast<AcTextView *>(widget(tab)); }

// - Actions -

void
TextEditTabView::appendText(const QString &text, int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabViews_.size());
  if (tab >= 0 && tab < tabCount())
    textWidget(tab)->append(text);
}

void
TextEditTabView::clear(int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabViews_.size());
  if (tab >= 0 && tab < tabCount())
    textWidget(tab)->clear();
}

// EOF
