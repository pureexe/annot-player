// filteredtableview.cc
// 11/17/2011

#include "filteredtableview.h"
#include "lineedit.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "global.h"
#include "core/gui/combobox.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

// - Constructions -

FilteredTableView::FilteredTableView(QStandardItemModel *sourceModel, QWidget *parent)
  : Base(parent), sourceModel_(sourceModel)
{
  Q_ASSERT(sourceModel_);
  setContentsMargins(0, 0, 0, 0);

  //setWindowTitle(tr("Process view"));
  //UiStyle::globalInstance()->setWindowStyle(this);

  // Create models

  createLayout();

  // Set initial states
  //proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  //filterColumnComboBox_->setCurrentIndex(HD_Name);
  proxyView_->sortByColumn(0);
  filterColumnComboBox_->setCurrentIndex(0);
  filterPatternLineEdit_->setFocus();
}

void
FilteredTableView::createLayout()
{
  //sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  //setProcessHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

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
    filterPatternLineEdit_->setStyleSheet(SS_LINEEDIT);
    filterPatternLineEdit_->setToolTip(TR(T_FILTER_PATTERN));
  }
  QLabel *filterPatternLabel = new QLabel; {
    filterPatternLabel->setStyleSheet(SS_LABEL);
    filterPatternLabel->setBuddy(filterPatternLineEdit_);
    filterPatternLabel->setText(TR(T_FILTER_PATTERN) + ":");
    filterPatternLabel->setToolTip(filterPatternLineEdit_->toolTip());
  }

  filterSyntaxComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterSyntaxComboBox_);
    filterSyntaxComboBox_->addItem(TR(T_FILTER_REGEX), QRegExp::RegExp);
    filterSyntaxComboBox_->addItem(TR(T_FILTER_WILDCARD), QRegExp::Wildcard);
    filterSyntaxComboBox_->addItem(TR(T_FILTER_FIXED), QRegExp::FixedString);
    filterSyntaxComboBox_->setToolTip(TR(T_FILTER_SYNTAX));
  }
  QLabel *filterSyntaxLabel = new QLabel; {
    filterSyntaxLabel->setStyleSheet(SS_LABEL);
    filterSyntaxLabel->setBuddy(filterSyntaxComboBox_);
    filterSyntaxLabel->setText(TR(T_FILTER_SYNTAX) + ":");
    filterSyntaxLabel->setToolTip(filterSyntaxComboBox_->toolTip());
  }

  filterColumnComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterColumnComboBox_);
    for (int i = 0; i < sourceModel_->columnCount(); i++)
      filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
    filterColumnComboBox_->setToolTip(TR(T_FILTER_COLUMN));
  }
  QLabel *filterColumnLabel = new QLabel; {
    filterColumnLabel->setStyleSheet(SS_LABEL);
    filterColumnLabel->setBuddy(filterColumnComboBox_);
    filterColumnLabel->setText(TR(T_FILTER_COLUMN) + ":");
    filterColumnLabel->setToolTip(filterColumnComboBox_->toolTip());
  }

  // Set layout

  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;

    layout->addWidget(proxyView_, r=0, c=0, 1, 3);

    layout->addWidget(filterPatternLabel, ++r, c=0);
    layout->addWidget(filterPatternLineEdit_, r, ++c, 1, 2);

    layout->addWidget(filterSyntaxLabel, ++r, c=0);
    layout->addWidget(filterSyntaxComboBox_, r, ++c, 1, 2);

    layout->addWidget(filterColumnLabel, ++r, c=0);
    layout->addWidget(filterColumnComboBox_, r, ++c, 1, 2);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  // Set up connections
  connect(filterPatternLineEdit_, SIGNAL(textChanged(QString)),
          SLOT(invalidateFilterRegExp()));
  connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterRegExp()));
  connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QModelIndex
FilteredTableView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
FilteredTableView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
}

void
FilteredTableView::setCurrentColumn(int col)
{ filterColumnComboBox_->setCurrentIndex(col); }

void
FilteredTableView::sortByColumn(int col, Qt::SortOrder order)
{ proxyView_->sortByColumn(col, order); }

// - Slots -


void
FilteredTableView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
}

void
FilteredTableView::invalidateFilterRegExp()
{
  QRegExp::PatternSyntax syntax =
      QRegExp::PatternSyntax(
        filterSyntaxComboBox_->itemData(
          filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternLineEdit_->text(), Qt::CaseInsensitive, syntax);
  proxyModel_->setFilterRegExp(regExp);
}

void
FilteredTableView::invalidateFilterColumn()
{ proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex()); }

// EOF
