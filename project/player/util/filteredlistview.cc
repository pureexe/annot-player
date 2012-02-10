// filteredlistview.cc
// 11/17/2011

#include "filteredlistview.h"
#include "comboedit.h"
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
  filterPatternEdit_->setFocus();
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

  filterPatternEdit_ = new ComboEdit; {
    filterPatternEdit_->setToolTip(TR(T_FILTER_PATTERN));
  }
  //QLabel *filterPatternLabel = new QLabel; {
  //  filterPatternLabel->setStyleSheet(SS_LABEL);
  //  filterPatternLabel->setBuddy(filterPatternEdit_);
  //  filterPatternLabel->setText(TR(T_FILTER_PATTERN) + ":");
  //  filterPatternLabel->setToolTip(filterPatternEdit_->toolTip());
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

  countButton_ = new QtExt::ToolButton; {
    countButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
    countButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
    countButton_->setText("0/0");
    countButton_->setToolTip(tr("Count"));
  } connect(countButton_, SIGNAL(clicked()), SLOT(popup()));

  // Set layout

  //QLayout *layout = new QVBoxLayout; {
  //  layout->addWidget(proxyView_);
  //  layout->addWidget(filterPatternEdit_);
//
  //  layout->setContentsMargins(0, 0, 0, 0);
  //  setContentsMargins(0, 0, 0, 0);
  //}
  //setLayout(layout);

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
          SLOT(invalidateFilterRegExp()));
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(invalidateCount()));
  //connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
  //        SLOT(invalidateFilterRegExp()));
  //connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
  //        SLOT(invalidateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QLineEdit*
FilteredListView::lineEdit() const
{ return filterPatternEdit_->lineEdit(); }

QModelIndex
FilteredListView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
FilteredListView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
  invalidateCount();
}

// - Actions -

void
FilteredListView::popup()
{ filterPatternEdit_->showPopup(); }

void
FilteredListView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
  invalidateCount();
}

void
FilteredListView::invalidateFilterRegExp()
{
  //QRegExp::PatternSyntax syntax =
  //    QRegExp::PatternSyntax(
  //      filterSyntaxComboBox_->itemData(
  //        filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternEdit_->currentText(), Qt::CaseInsensitive, QRegExp::FixedString);
  proxyModel_->setFilterRegExp(regExp);
  invalidateCount();
}

//void
//FilteredListView::invalidateFilterColumn()
//{ proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex()); }

void
FilteredListView::invalidateCount()
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
