// textedittabview.cc
// 1/1/2012

#include "textedittabview.h"
#include "textview.h"
#include "tr.h"
#include "stylesheet.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/toolbuttonwithid.h"
#include <QtGui>

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

// - Constructions -

void
TextEditTabView::initializeLayout()
{ stackLayout_ = new QStackedLayout; }

void
TextEditTabView::finalizeLayout()
{
  //QToolButton *clearButton = new QtExt::ToolButton; {
  //  clearButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
  //  clearButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  //  clearButton->setText(QString("[ %1 ]").arg(TR(T_CLEAR)));
  //  clearButton->setToolTip(TR(T_CLEAR));
  //  connect(clearButton, SIGNAL(clicked()), SLOT(clear()));
  //}

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(stackLayout_);

    foreach (QToolButton *b, tabButtons_)
      header->addWidget(b);
    //header->addStretch();
    //header->addWidget(clearButton);

    stackLayout_->setContentsMargins(0, 0, 0, 0);
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

  for (int i = 0; i < qMin(tabCount_,10); i++) {
    QShortcut *c = new QShortcut(QKeySequence("CTRL+" + QString::number(i+1)), this);
    connect(c, SIGNAL(activated()), tabButtons_[i], SLOT(click()));
  }

  if (tabCount_ > 0)
    setTab(0);
  else
    tabIndex_ = -1;
}

void
TextEditTabView::addTab(const QString &tabName)
{
  QtExt::ToolButtonWithId *tabButton = new QtExt::ToolButtonWithId(tabCount_); {
    tabButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    tabButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    tabButton->setText(QString("- %1 -").arg(tabName));
    tabButton->setToolTip(
      QString("%1 [" K_CTRL "+%2]").arg(tabName).arg(QString::number(tabCount_+1))
    );
    tabButton->setCheckable(true);
    connect(tabButton, SIGNAL(clickedWithId(int)), SLOT(setTab(int)));
  }
  tabButtons_.append(tabButton);

  TextView *tabView = new TextView;
  tabViews_.append(tabView);

  stackLayout_->addWidget(tabView);
  tabCount_++;
}

// - Properties -

void
TextEditTabView::appendText(const QString &text, int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabViews_.size());
  if (tab >= 0 && tab < tabViews_.size())
    tabViews_[tab]->append(text);
}

void
TextEditTabView::clear(int tab)
{
  Q_ASSERT(tab >= 0 && tab < tabViews_.size());
  if (tab >= 0 && tab < tabViews_.size())
    tabViews_[tab]->clear();
}

void
TextEditTabView::invalidateTabIndex()
{
  stackLayout_->setCurrentIndex(tabIndex_);
  for (int i = 0; i < tabButtons_.size(); i++)
    tabButtons_[i]->setChecked(tabIndex_ == i);
}

// EOF
