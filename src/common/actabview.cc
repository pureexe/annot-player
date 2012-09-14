// actabview.cc
// 1/1/2012

#include "src/common/actabview.h"
#include "src/common/acss.h"
#include "src/common/acui.h"
#include "lib/qtext/toolbutton.h"
#include "lib/qtext/toolbuttonwithid.h"
#include "lib/qtext/compactstackedlayout.h"
#include <QtGui>

#ifdef Q_OS_MAC
# define K_CTRL        "cmd"
#else
# define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

// - Constructions -

void
AcTabView::initializeLayout()
{ stackLayout_ = new CompactStackedLayout; }

void
AcTabView::finalizeLayout()
{
  //QToolButton *clearButton = new QtExt::ToolButton; {
  //  clearButton->setStyleSheet(ACSS_TOOLBUTTON_TEXT);
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

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  new QShortcut(QKeySequence("CTRL+0"), tabButtons_[tabCount_ - 1], SLOT(click()));
#ifndef Q_OS_MAC
  new QShortcut(QKeySequence("ALT+0"), tabButtons_[tabCount_ - 1], SLOT(click()));
#endif // Q_OS_MAC
  for (int i = 0; i < qMin(tabCount_,8); i++) {
    new QShortcut(QKeySequence("CTRL+" + QString::number(i+1)), tabButtons_[i], SLOT(click()));
#ifndef Q_OS_MAC
    new QShortcut(QKeySequence("ALT+" + QString::number(i+1)), tabButtons_[i], SLOT(click()));
#endif // Q_OS_MAC
  }

  new QShortcut(QKeySequence("CTRL+TAB"), this, SLOT(nextTab()));
  new QShortcut(QKeySequence("CTRL+SHIFT+TAB"), this, SLOT(previousTab()));
  new QShortcut(QKeySequence("CTRL+}"), this, SLOT(nextTab()));
  new QShortcut(QKeySequence("CTRL+{"), this, SLOT(previousTab()));

  if (tabCount_ > 0)
    setTab(0);
  else
    tabIndex_ = -1;
}

void
AcTabView::addTab(QWidget *tab)
{
  QString tabName = tab->windowTitle();
  QtExt::ToolButtonWithId *tabButton = new QtExt::ToolButtonWithId(tabCount_); {
    tabButton->setStyleSheet(ACSS_TOOLBUTTON_TEXT_TAB);
    tabButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    tabButton->setText(QString("- %1 -").arg(tabName));
    tabButton->setToolTip(
      QString("%1 [" K_CTRL "+%2]").arg(tabName).arg(QString::number(tabCount_+1))
    );
    tabButton->setCheckable(true);
    tabButton->setGraphicsEffect(AcUi::globalInstance()->makeHaloEffect(AcUi::TabHint));
    connect(tabButton, SIGNAL(clickedWithId(int)), SLOT(setTab(int)));
  }
  tabButtons_.append(tabButton);

  tabs_.append(tab);
  stackLayout_->addWidget(tab);
  tabCount_++;
}

// - Actions -

QSize
AcTabView::sizeHint() const
{
  QSize base = Base::sizeHint();
  QSize current;
  if (QWidget *w = currentWidget())
    current = w->sizeHint();
  int headerHeight = 0;
  if (!tabButtons_.isEmpty())
    headerHeight = tabButtons_.first()->sizeHint().height();

  int w = qMax(base.width(), current.width());
  int h = current.height() + headerHeight;
  return QSize(w, h);
}

void
AcTabView::invalidateTabIndex()
{
  stackLayout_->setCurrentIndex(tabIndex_);
  for (int i = 0; i < tabButtons_.size(); i++)
    tabButtons_[i]->setChecked(tabIndex_ == i);
  emit tabChanged(tabIndex_);
}

void
AcTabView::setTab(int index)
{
  if (index >= 0 && index < tabCount_) {
    tabIndex_ = index;
    invalidateTabIndex();
  }
}

void
AcTabView::previousTab()
{
  if (tabCount_) {
    if (--tabIndex_ < 0)
      tabIndex_ = tabCount_ - 1;
    invalidateTabIndex();
  }
}

void
AcTabView::nextTab()
{
  if (tabCount_) {
    if (++tabIndex_ >= tabCount_)
      tabIndex_ = 0;
    invalidateTabIndex();
  }
}

// EOF
