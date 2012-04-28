// acfilteredlistview.cc
// 11/17/2011

#include "project/common/acfilteredlistview.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#include <QtGui>

// - Constructions -

AcFilteredListView::AcFilteredListView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent)
  : Base(parent), sourceModel_(sourceModel), proxyModel_(proxyModel)
{
  Q_ASSERT(sourceModel_);
  Q_ASSERT(proxyModel_);

  // Create models

  createLayout();

  // Set initial states
  //proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  //filterColumnComboBox_->setCurrentIndex(HD_Name);
  proxyView_->sortByColumn(0);
  //filterColumnComboBox_->setCurrentIndex(0);
  filterPatternEdit_->setFocus();
}

void
AcFilteredListView::createLayout()
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

  proxyView_ = new QTreeView; {
    proxyView_->setStyleSheet(SS_TREEVIEW);
    proxyView_->setRootIsDecorated(true);
    proxyView_->setAlternatingRowColors(true);
    proxyView_->setSortingEnabled(true);
    proxyView_->setModel(proxyModel_);
    proxyView_->setToolTip(tr("View"));
  }

  filterPatternEdit_ = ui->makeComboBox(AcUi::EditHint, "", tr("Filter pattern"), tr("Search"));
  countButton_ = ui->makeToolButton(0, "0/0", tr("Count"), this, SLOT(popup()));

  // Set layout
  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;

    layout->addWidget(proxyView_, r=0, c=0, 1, 3);

    layout->addWidget(filterPatternEdit_, ++r, c=0, 1, 3);
    layout->addWidget(countButton_, r, c+=2, 1, 1, Qt::AlignRight);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(layout);

  // Set up connections
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(updateFilterRegExp()));
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(updateCount()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QLineEdit*
AcFilteredListView::lineEdit() const
{ return filterPatternEdit_->lineEdit(); }

QModelIndex
AcFilteredListView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
AcFilteredListView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
  updateCount();
}

// - Actions -

void
AcFilteredListView::sortByColumn(int col)
{ proxyView_->sortByColumn(col); }

void
AcFilteredListView::popup()
{ filterPatternEdit_->showPopup(); }

void
AcFilteredListView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
  updateCount();
}

void
AcFilteredListView::updateFilterRegExp()
{
  //QRegExp::PatternSyntax syntax =
  //    QRegExp::PatternSyntax(
  //      filterSyntaxComboBox_->itemData(
  //        filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternEdit_->currentText(), Qt::CaseInsensitive, QRegExp::FixedString);
  proxyModel_->setFilterRegExp(regExp);
  updateCount();
}

//void
//AcFilteredListView::updateFilterColumn()
//{ proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex()); }

void
AcFilteredListView::updateCount()
{
  int total = sourceModel_->rowCount();
  int count = proxyModel_->rowCount();
  countButton_->setText(
    QString("%1/%2")
      .arg(QString::number(count))
      .arg(QString::number(total))
  );
}

// EOF
