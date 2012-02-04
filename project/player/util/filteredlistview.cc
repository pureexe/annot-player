// filteredlistview.cc
// 11/17/2011

#include "filteredlistview.h"
#include "lineedit.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "defines.h"
//#include "module/qtext/combobox.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

// - Constructions -

FilteredListView::FilteredListView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent)
  : Base(parent), sourceModel_(sourceModel), proxyModel_(proxyModel)
{
  Q_ASSERT(sourceModel_);
  Q_ASSERT(proxyModel_);

  //setWindowTitle(tr("Process view"));
  //UiStyle::globalInstance()->setWindowStyle(this);

  // Create models

  createLayout();

  // Set initial states
  //proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  //filterColumnComboBox_->setCurrentIndex(HD_Name);
  proxyView_->sortByColumn(0);
  //filterColumnComboBox_->setCurrentIndex(0);
  filterPatternLineEdit_->setFocus();
}

void
FilteredListView::createLayout()
{
  //sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  //setProcessHeaderData(sourceModel_);

  //proxyModel_ = new QSortFilterProxyModel; {
  //  proxyModel_->setSourceModel(sourceModel_);
  //  proxyModel_->setDynamicSortFilter(true);
  //  proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  //}

  // Create widgets

  proxyView_ = new QTreeView; {
    proxyView_->setStyleSheet(SS_TREEVIEW);
    proxyView_->setRootIsDecorated(true);
    proxyView_->setAlternatingRowColors(true);
    proxyView_->setSortingEnabled(true);
    proxyView_->setModel(proxyModel_);
    //proxyView_->setToolTip(tr("Running processes"));
  }

  filterPatternLineEdit_ = new LineEdit; {
    filterPatternLineEdit_->setToolTip(TR(T_FILTER_PATTERN));
  }
  //QLabel *filterPatternLabel = new QLabel; {
  //  filterPatternLabel->setStyleSheet(SS_LABEL);
  //  filterPatternLabel->setBuddy(filterPatternLineEdit_);
  //  filterPatternLabel->setText(TR(T_FILTER_PATTERN) + ":");
  //  filterPatternLabel->setToolTip(filterPatternLineEdit_->toolTip());
  //}

  //filterSyntaxComboBox_ = new QtExt::ComboBox; {
  //  UiStyle::globalInstance()->setComboBoxStyle(filterSyntaxComboBox_);
  //  filterSyntaxComboBox_->addItem(TR(T_FILTER_REGEX), QRegExp::RegExp);
  //  filterSyntaxComboBox_->addItem(TR(T_FILTER_WILDCARD), QRegExp::Wildcard);
  //  filterSyntaxComboBox_->addItem(TR(T_FILTER_FIXED), QRegExp::FixedString);
  //  filterSyntaxComboBox_->setToolTip(TR(T_FILTER_SYNTAX));
  //}
  //QLabel *filterSyntaxLabel = new QLabel; {
  //  filterSyntaxLabel->setStyleSheet(SS_LABEL);
  //  filterSyntaxLabel->setBuddy(filterSyntaxComboBox_);
  //  filterSyntaxLabel->setText(TR(T_FILTER_SYNTAX) + ":");
  //  filterSyntaxLabel->setToolTip(filterSyntaxComboBox_->toolTip());
  //}

  //filterColumnComboBox_ = new QtExt::ComboBox; {
  //  UiStyle::globalInstance()->setComboBoxStyle(filterColumnComboBox_);
  //  for (int i = 0; i < sourceModel_->columnCount(); i++)
  //    filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
  //  filterColumnComboBox_->setToolTip(TR(T_FILTER_COLUMN));
  //}
  //QLabel *filterColumnLabel = new QLabel; {
  //  filterColumnLabel->setStyleSheet(SS_LABEL);
  //  filterColumnLabel->setBuddy(filterColumnComboBox_);
  //  filterColumnLabel->setText(TR(T_FILTER_COLUMN));
  //  filterColumnLabel->setToolTip(filterColumnComboBox_->toolTip());
  //}

  // Set layout

  QLayout *layout = new QVBoxLayout; {
    layout->addWidget(proxyView_);
    layout->addWidget(filterPatternLineEdit_);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  // Set up connections
  connect(filterPatternLineEdit_, SIGNAL(textChanged(QString)),
          SLOT(invalidateFilterRegExp()));
  //connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
  //        SLOT(invalidateFilterRegExp()));
  //connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
  //        SLOT(invalidateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QModelIndex
FilteredListView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
FilteredListView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
}

// - Slots -

void
FilteredListView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
}

void
FilteredListView::invalidateFilterRegExp()
{
  //QRegExp::PatternSyntax syntax =
  //    QRegExp::PatternSyntax(
  //      filterSyntaxComboBox_->itemData(
  //        filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternLineEdit_->text(), Qt::CaseInsensitive, QRegExp::FixedString);
  proxyModel_->setFilterRegExp(regExp);
}

//void
//FilteredListView::invalidateFilterColumn()
//{ proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex()); }

// EOF
