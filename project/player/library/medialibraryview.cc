// medialibraryview.cc
// 8/5/2012

#include "medialibraryview.h"
#include "medialibrary.h"
#include "mediastandardmodel.h"
#include "tr.h"
#include "global.h"
#include "project/common/acui.h"
#include "project/common/acss.h"
#include "module/qtext/layoutwidget.h"
#include "module/qtext/overlaylayout.h"
#include <QtGui>

//#define DEBUG "medialibraryview"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
# define K_CTRL "cmd"
#else
# define K_CTRL "Ctrl"
#endif // Q_OS_MAC

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint)

MediaLibraryView::MediaLibraryView(MediaLibrary *library, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), library_(library)
{
  Q_ASSERT(library_);
  setWindowTitle(tr("Library"));

  createLayout();

  listView_->sortByColumn(MediaStandardModel::HD_Location);

  resize(800, 400);
}

void
MediaLibraryView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  // Create models
  QStandardItemModel *standardModel = library_->standardModel();

  standardFilterModel_ = new QSortFilterProxyModel; {
    standardFilterModel_->setSourceModel(standardModel);
    standardFilterModel_->setDynamicSortFilter(true);
    standardFilterModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    standardFilterModel_->setFilterKeyColumn(MediaStandardModel::HD_Type);
  }

  standardProxyModel_ = new QSortFilterProxyModel; {
    standardProxyModel_->setSourceModel(standardFilterModel_);
    standardProxyModel_->setDynamicSortFilter(true);
    standardProxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    standardProxyModel_->setFilterKeyColumn(-1);
  }

  standardSelectionModel_ = new QItemSelectionModel(standardProxyModel_); // shared selections
  connect(standardSelectionModel_, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(showSelection()));
  connect(standardSelectionModel_, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateButtons()));

  // Create views

  iconView_ = new QListView(this); {
    iconView_->setViewMode(QListView::IconMode);
    iconView_->setStyleSheet(ACSS_LISTVIEW);
    //listView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //listView_->setAlternatingRowColors(false);
    //listView_->viewport()->setAttribute(Qt::WA_StaticContents);
    //listView_->setAttribute(Qt::WA_MacShowFocusRect, false);
    //iconView_->setWrapping(true);
    //iconView_->setWordWrap(false);
    iconView_->setGridSize(QSize(260, 60)); // (260*3 = 780) < 800 - margin

    iconView_->setModel(standardProxyModel_);
    iconView_->setSelectionModel(standardSelectionModel_);
  }

  listView_ = new QTreeView(this); {
    listView_->setStyleSheet(ACSS_TREEVIEW);
    listView_->setRootIsDecorated(true);
    listView_->setSortingEnabled(true);
    listView_->setAlternatingRowColors(true);
    //tableView_->horizontalHeader()->setMovable(true);
    //tableView_->verticalHeader()->setMovable(true);
    //tableView_->viewport()->setAttribute(Qt::WA_StaticContents); // Set StaticContents to enable minimal repaints on resizes.
    listView_->setColumnWidth(0, 350);

    listView_->setModel(standardProxyModel_);
    listView_->setSelectionModel(standardSelectionModel_);
  }

  treeView_ = new QTreeView(this); {
    treeView_->setStyleSheet(ACSS_TREEVIEW);
    //treeView_->setRootIsDecorated(true);
    treeView_->setSortingEnabled(true);
    treeView_->setAlternatingRowColors(true);
    //treeView_->setUniformRowHeights(true);
    //treeView_->header()->setStretchLastSection(false);
    //treeView_->viewport()->setAttribute(Qt::WA_StaticContents);
    //treeView_->setAttribute(Qt::WA_MacShowFocusRect, false); // Disable the focus rect to get minimal repaints when scrolling on Mac.

    //treeView_->setModel(standardProxyModel_);
    //treeView_->setSelectionModel(standardSelectionModel_);
  }

  connect(iconView_, SIGNAL(doubleClicked(QModelIndex)), SLOT(open()));
  connect(listView_, SIGNAL(doubleClicked(QModelIndex)), SLOT(open()));
  connect(treeView_, SIGNAL(doubleClicked(QModelIndex)), SLOT(open()));

  // Create header widgets

  countButton_ = ui->makeToolButton(0, "0/0", tr("Count"));

  filterEdit_ = ui->makeLineEdit(0, QString(), tr("Search with Regular Expression"), tr("Search"));
  connect(filterEdit_, SIGNAL(textEdited(QString)), SLOT(updateFilterEdit()));

  // Must be consistent with FilterType enum.
  filterType_ = ui->makeComboBox(AcUi::ReadOnlyHint, QString(), tr("Type")); {
    filterType_->setMinimumWidth(70);
    filterType_->addItem(tr("All"));
    filterType_->addItem(tr("Game"));
    filterType_->addItem(tr("Video"));
    //filterType_->addItem(tr("Audio"));
    filterType_->addItem(tr("URL"));
  } connect(filterType_, SIGNAL(activated(int)), SLOT(setFilterType(int)));

  dirButton_ = ui->makeToolButton(AcUi::PushHint, tr("Dir"), tr("Open Directory"), this, SLOT(openDirectory()));
  openButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, TR(T_PLAY), TR(T_PLAY), tr("Double Click"), this, SLOT(open()));

  QToolButton *refreshButton = ui->makeToolButton(AcUi::PushHint, TR(T_REFRESH), tr("Rebuild the library index"), K_CTRL "+R", this, SLOT(refresh())),
              *clearButton = ui->makeToolButton(AcUi::PushHint, TR(T_CLEAR),tr("Clear library"), this, SLOT(clear()));

  new QShortcut(QKeySequence("CTRL+R"), refreshButton, SLOT(click()));

  QRadioButton *iconButton = ui->makeRadioButton(0, QString(), tr("Icon View"), K_CTRL "+1"),
               *listButton = ui->makeRadioButton(0, QString(), tr("List View"), K_CTRL "+2"),
               *treeButton = ui->makeRadioButton(0, QString(), tr("Tree View"), K_CTRL "+3");
  iconButton->setChecked(true);

  connect(iconButton, SIGNAL(clicked()), SLOT(showIcon()));
  connect(listButton, SIGNAL(clicked()), SLOT(showList()));
  connect(treeButton, SIGNAL(clicked()), SLOT(showTree()));

  new QShortcut(QKeySequence("CTRL+1"), iconButton, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+2"), listButton, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+3"), treeButton, SLOT(click()));
#ifndef Q_OS_MAC
  new QShortcut(QKeySequence("ALT+1"), iconButton, SLOT(click()));
  new QShortcut(QKeySequence("ALT+2"), listButton, SLOT(click()));
  new QShortcut(QKeySequence("ALT+3"), treeButton, SLOT(click()));
#endif // Q_OS_MAC

  // Create layout

  views_ = new QStackedLayout;
  views_->addWidget(iconView_);
  views_->addWidget(listView_);
  views_->addWidget(treeView_);

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(views_);
    rows->addLayout(footer);

    //header->addWidget(refreshButton);
    //header->addWidget(clearButton);
    //header->addStretch();

    header->addWidget(iconButton);
    header->addWidget(listButton);
    header->addWidget(treeButton);
    header->addStretch();
    header->addWidget(filterType_);

    OverlayLayout *overlay = new OverlayLayout;
    overlay->addWidget(filterEdit_);
    overlay->addWidget(countButton_, Qt::AlignRight);
    header->addLayout(overlay);

    footer->addWidget(refreshButton);
    footer->addWidget(clearButton);
    footer->addStretch();
    footer->addWidget(dirButton_);
    footer->addWidget(openButton_);

    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;
    rows->setContentsMargins(patch, patch, patch, patch);
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    views_->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }  setCentralWidget(new LayoutWidget(rows, this));
}

// - Actions -

void
MediaLibraryView::showIcon()
{ views_->setCurrentWidget(iconView_); }

void
MediaLibraryView::showList()
{ views_->setCurrentWidget(listView_); }

void
MediaLibraryView::showTree()
{ views_->setCurrentWidget(treeView_); }

void
MediaLibraryView::refresh()
{
  //showMessage(tr("refreshing library"));
  library_->refresh();
  updateCount();
  updateButtons();
  //showMessage(tr("library refreshed"));
}

void
MediaLibraryView::clear(bool confirm)
{
  if (confirm &&
      QMessageBox::Ok != QMessageBox::question(this,
        tr("Confirm"),  // title
        tr("Do you want to clear the media library?"),  // emssage
        QMessageBox::Ok | QMessageBox::Cancel,  // choices
        QMessageBox::Cancel // default choice
      ))
    return;
  library_->clear();
  library_->refresh();
  updateCount();
}

void
MediaLibraryView::open()
{
  if (standardSelectionModel_->hasSelection())  {
    QModelIndex index = standardSelectionModel_->currentIndex();
    QString location = MediaStandardModel::indexLocation(index);
    if (!location.isEmpty()) {
      showMessage(tr("opening") + ": " + location);
      emit openRequested(location);
    }
  }
}

void
MediaLibraryView::showSelection()
{
  if (standardSelectionModel_->hasSelection())  {
    QModelIndex index = standardSelectionModel_->currentIndex();
    QString location = MediaStandardModel::indexLocation(index);
    if (!location.isEmpty())
      showMessage(location);
  }
}

void
MediaLibraryView::openDirectory()
{
  if (standardSelectionModel_->hasSelection())  {
    QModelIndex index = standardSelectionModel_->currentIndex();
    QString location = MediaStandardModel::indexLocation(index);
    if (!location.isEmpty()) {
      QFileInfo fi(location);
      fi = QFileInfo(fi.absolutePath());
      QString path = fi.absoluteFilePath();
      if (!fi.exists()) {
        warn(tr("not exist") + ": " + path);
        return;
      }
      showMessage(tr("opening") + ": " + path);
      if (!path.contains("://")) {
#ifdef Q_OS_WIN
        path.replace('\\', '/');
        path.prepend("file:///");
#else
        path.prepend("file://");
#endif // Q_OS_WIN
      }
      QDesktopServices::openUrl(path);
    }
  }
}

// - Filters -

void
MediaLibraryView::updateFilterEdit()
{
  standardProxyModel_->setFilterRegExp(
    QRegExp(filterEdit_->text().trimmed(), Qt::CaseInsensitive)
  );
  updateCount();
}

// Must be consistent with FORMAT_TYPE in mediaitemmodel.cc
//#define FORMAT_TYPE(_media)
//(
//  (_media).isGame() ? : TR(T_GALGAME)
//  (_media).isRemote() ? TR(T_URL) :
//  (_media).isVideo() ? TR(T_VIDEO) :
//  (_media).isAudio() ? TR(T_AUDIO) :
//                       TR(T_FILE)
//)
void
MediaLibraryView::setFilterType(int type)
{
  QString key;
  switch (type) {
  case FT_Game:  key = TR(T_GALGAME); break;
  case FT_Video: key = TR(T_VIDEO); break;
  case FT_Url:   key = TR(T_URL); break;
  }
  standardFilterModel_->setFilterFixedString(key);
  updateCount();
}

// - Events -

void
MediaLibraryView::updateButtons()
{
  bool t = standardSelectionModel_->hasSelection();
  openButton_->setEnabled(t);
  dirButton_->setEnabled(t);
}

void
MediaLibraryView::updateCount()
{
  int total = library_->standardModel()->rowCount(),
      count = standardProxyModel_->rowCount();
  countButton_->setText(
    QString("%1/%2")
      .arg(QString::number(count))
      .arg(QString::number(total))
  );
}

void
MediaLibraryView::setVisible(bool visible)
{
  if (visible && library_->isDirty())
    refresh();
  Base::setVisible(visible);
}

// EOF
