// textedittabview.cc
// 1/1/2012

#include "textedittabview.h"
#include "textview.h"
#include "qtx/qxtoolbutton.h"
#include "qtx/qxtoolbuttonwithid.h"
#include <QtGui>

// - Constructions -

void
TextEditTabView::addTab(const QString &tabName)
{
  QWidget *tab = new TextView(this);
  tab->setWindowTitle(tabName);
  Base::addTab(tab);
}

TextView*
TextEditTabView::textWidget(int tab) const
{ return qobject_cast<TextView *>(widget(tab)); }

// - Actions -

void
TextEditTabView::appendText(const QString &text, int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabCount());
  if (tab >= 0 && tab < tabCount())
    textWidget(tab)->append(text);
}

void
TextEditTabView::clear(int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabCount());
  if (tab >= 0 && tab < tabCount())
    textWidget(tab)->clear();
}

// EOF
