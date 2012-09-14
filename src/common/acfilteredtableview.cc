// acfilteredtableview.cc
// 11/17/2011

#include "src/common/acfilteredtableview.h"
#include "src/common/acss.h"
#include "src/common/acui.h"
#include <QtGui>

// - Constructions -

AcFilteredTableView::AcFilteredTableView(QAbstractItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent)
  : Base(parent), sourceModel_(sourceModel), proxyModel_(proxyModel)
{
  Q_ASSERT(sourceModel_);
  Q_ASSERT(proxyModel_);
  setContentsMargins(0, 0, 0, 0);

  // Create models

  createLayout();

  // Set initial states
  //proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  //filterColumnComboBox_->setCurrentIndex(HD_Name);
  proxyView_->sortByColumn(0);
  filterColumnComboBox_->setCurrentIndex(0);
  filterPatternEdit_->setFocus();
}

void
AcFilteredTableView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  //sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  //setProcessHeaderData(sourceModel_);

  //proxyModel_ = new QSortFilterProxyModel; {
  //  proxyModel_->setSourceModel(sourceModel_);
  //  proxyModel_->setDynamicSortFilter(true);
  //  proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  //}

  // Create widgets

  proxyView_ = new QTreeView(this); {
    proxyView_->setStyleSheet(ACSS_TREEVIEW);
    proxyView_->setRootIsDecorated(true);
    proxyView_->setAlternatingRowColors(true);
    proxyView_->setSortingEnabled(true);
    proxyView_->setToolTip(tr("Table"));

    proxyView_->setModel(proxyModel_);
  }

  filterPatternEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Filter Pattern"), tr("Search"));
  QLabel *filterPatternLabel = ui->makeLabel(
        AcUi::BuddyHint,tr("Filter Pattern"), filterPatternEdit_);

  filterSyntaxComboBox_ = ui->makeComboBox(AcUi::ReadOnlyHint, "", tr("Filter Syntax")); {
    filterSyntaxComboBox_->addItem(tr("Regular Expression"), QRegExp::RegExp);
    filterSyntaxComboBox_->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox_->addItem(tr("Fixed String"), QRegExp::FixedString);
  }
  QLabel *filterSyntaxLabel = ui->makeLabel(
        AcUi::BuddyHint, tr("Filter Syntax"), filterSyntaxComboBox_);

  filterColumnComboBox_ = ui->makeComboBox(AcUi::ReadOnlyHint, "", tr("Filter Column")); {
    for (int i = 0; i < sourceModel_->columnCount(); i++)
      filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
  }
  QLabel *filterColumnLabel = ui->makeLabel(
        AcUi::BuddyHint, tr("Filter Column"), filterColumnComboBox_);

  countButton_ = ui->makeToolButton(0, "0/0", tr("Count"), this, SLOT(popup()));

  // Set layout

  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;

    layout->addWidget(proxyView_, r=0, c=0, 1, 3);

    layout->addWidget(filterPatternLabel, ++r, c=0);
    layout->addWidget(filterPatternEdit_, r, ++c, 1, 2);
    layout->addWidget(countButton_, r, ++c, 1, 1, Qt::AlignRight);

    layout->addWidget(filterSyntaxLabel, ++r, c=0);
    layout->addWidget(filterSyntaxComboBox_, r, ++c, 1, 2);

    layout->addWidget(filterColumnLabel, ++r, c=0);
    layout->addWidget(filterColumnComboBox_, r, ++c, 1, 2);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  // Set up connections
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(updateFilterRegExp()));
  //connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
  //        SLOT(updateCount()));
  connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(updateFilterRegExp()));
  connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(updateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QModelIndex
AcFilteredTableView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
AcFilteredTableView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
  updateCount();
}

void
AcFilteredTableView::setCurrentColumn(int col)
{ filterColumnComboBox_->setCurrentIndex(col); }

void
AcFilteredTableView::sortByColumn(int col, Qt::SortOrder order)
{ proxyView_->sortByColumn(col, order); }

// - Actions -

void
AcFilteredTableView::popup()
{ filterPatternEdit_->showPopup(); }

void
AcFilteredTableView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
  updateCount();
}

void
AcFilteredTableView::updateFilterRegExp()
{
  QRegExp::PatternSyntax syntax =
      QRegExp::PatternSyntax(
        filterSyntaxComboBox_->itemData(
          filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternEdit_->currentText(), Qt::CaseInsensitive, syntax);
  proxyModel_->setFilterRegExp(regExp);
  updateCount();
}

void
AcFilteredTableView::updateFilterColumn()
{
  proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex());
  updateCount();
}

void
AcFilteredTableView::updateCount()
{
  int total = sourceModel_->rowCount(),
      count = proxyModel_->rowCount();
  countButton_->setText(
    QString("%1/%2")
      .arg(QString::number(count))
      .arg(QString::number(total))
  );
}

// EOF
