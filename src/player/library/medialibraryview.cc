// medialibraryview.cc
// 8/5/2012

#include "medialibraryview.h"
#include "medialibrary.h"
#include "mediamodel.h"
#include "media.h"
#include "tr.h"
#include "global.h"
#include "rc.h"
#include "src/common/acui.h"
#include "src/common/acss.h"
#include "qtx/qxlayoutwidget.h"
#include "qtx/qxoverlaylayout.h"
#include <QtGui>

//#define DEBUG "medialibraryview"
#include "qtx/qxdebug.h"

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

#define SS_HEADER_BUTTON \
  ACSS_TOOLBUTTON_TEXT_NORMAL \
  SS_BEGIN(QToolButton) \
    SS_NO_MARGIN \
    SS_NO_PADDING \
  SS_END

MediaLibraryView::MediaLibraryView(MediaLibrary *library, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), library_(library)
{
  Q_ASSERT(library_);
  setWindowTitle(tr("Library"));

  createLayout();
  createActions();

  connect(this, SIGNAL(toggled()), SLOT(hide()));

  listView_->sortByColumn(MediaModel::HD_Location, Qt::AscendingOrder);
  listView_->sortByColumn(MediaModel::HD_TypeNo, Qt::AscendingOrder);

  resize(800, 400);
}

void
MediaLibraryView::focus()
{ filterEdit_->setFocus(); }

void
MediaLibraryView::createActions()
{
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true

#ifdef AC_ENABLE_GAME
  showGameAct_ = contextMenu_->addAction(tr("Game Preferences"), this, SLOT(showGame()));
  syncGameAct_ = contextMenu_->addAction(tr("Sync with Running Galgame"), this, SLOT(syncGame()));
  contextMenu_->addSeparator();
#endif // AC_ENABLE_GAME

  openAct_ = contextMenu_->addAction(QIcon(RC_IMAGE_PLAY), TR(T_OPEN), this, SLOT(open()));
  browseAct_ = contextMenu_->addAction(tr("Browse"), this, SLOT(browse()));
  contextMenu_->addSeparator();

  contextMenu_->addAction(tr("Hide View"), this, SLOT(fadeOut()));
  toggleAct_ = contextMenu_->addAction(tr("Toggle View"), this, SIGNAL(toggled()));
  contextMenu_->addSeparator();

  autoHideAct_ = contextMenu_->addAction(tr("Hide After Opening Media"));
  autoHideAct_->setCheckable(true);
  autoHideAct_->setChecked(true);

  autoRunAct_ = contextMenu_->addAction(tr("Show On Startup"));
  autoRunAct_->setCheckable(true);
  autoRunAct_->setChecked(true);
}

void
MediaLibraryView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  // Create models
  QStandardItemModel *sourceModel = library_->model();

  filterModel_ = new QSortFilterProxyModel; {
    filterModel_->setSourceModel(sourceModel);
    filterModel_->setDynamicSortFilter(true);
    filterModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterModel_->setFilterKeyColumn(MediaModel::HD_Type);
  }

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(filterModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel_->setFilterKeyColumn(-1);
  }

  selectionModel_ = new QItemSelectionModel(proxyModel_); // shared selections
  connect(selectionModel_, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(showSelection()));
  connect(selectionModel_, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateButtons()));

  // Create views

  iconView_ = new QListView(this); {
    iconView_->setViewMode(QListView::IconMode);
    iconView_->setStyleSheet(ACSS_LISTVIEW);
    iconView_->setResizeMode(QListView::Adjust);
    //iconView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //iconView_->setAlternatingRowColors(false);
    //iconView_->viewport()->setAttribute(Qt::WA_StaticContents);
    //iconView_->setAttribute(Qt::WA_MacShowFocusRect, false);
    //iconView_->setWrapping(true);
    //iconView_->setWordWrap(false);
    iconView_->setGridSize(QSize(260, 60)); // (260*3 = 780) < 800 - margin

    iconView_->setModel(proxyModel_);
    iconView_->setSelectionModel(selectionModel_);
  }

  listView_ = new QTreeView(this); {
    listView_->setStyleSheet(ACSS_TREEVIEW);
    listView_->setRootIsDecorated(true);
    listView_->setSortingEnabled(true);
    listView_->setAlternatingRowColors(true);
    //tableView_->horizontalHeader()->setMovable(true);
    //tableView_->verticalHeader()->setMovable(true);
    //tableView_->viewport()->setAttribute(Qt::WA_StaticContents); // Set StaticContents to enable minimal repaints on resizes.
    listView_->setModel(proxyModel_);
    listView_->setSelectionModel(selectionModel_);
    listView_->setColumnWidth(0, 350);
  }

  connect(iconView_, SIGNAL(doubleClicked(QModelIndex)), SLOT(open()));
  connect(listView_, SIGNAL(doubleClicked(QModelIndex)), SLOT(open()));

  // Create header widgets

  countButton_ = ui->makeToolButton(0, "0/0", tr("Count"));

  filterEdit_ = ui->makeLineEdit(0, QString(), tr("Search with regular expression splitted by spaces"), tr("Search"));
  connect(filterEdit_, SIGNAL(textEdited(QString)), SLOT(updateFilterEdit()));

  // Must be consistent with FilterType enum.
  filterType_ = ui->makeComboBox(AcUi::ReadOnlyHint, QString(), tr("Type")); {
    filterType_->setMinimumWidth(70);
    filterType_->addItem(tr("All"));
    filterType_->addItem(TR(T_FOLDER));
    filterType_->addItem(TR(T_VIDEO));
    //filterType_->addItem(TR(T_Audio));
    filterType_->addItem(TR(T_URL));
#ifdef AC_ENABLE_GAME
    filterType_->addItem(TR(T_GALGAME));
#endif // AC_ENABLE_GAME
  } connect(filterType_, SIGNAL(activated(int)), SLOT(setFilterType(int)));

  browseButton_ = ui->makeToolButton(AcUi::PushHint, tr("Browse"), tr("Browse location"), this, SLOT(browse()));
  openButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, TR(T_PLAY), TR(T_PLAY), tr("Double Click"), this, SLOT(open()));

  QToolButton *refreshButton = ui->makeToolButton(AcUi::PushHint, TR(T_REFRESH), tr("Rebuild the library index"), K_CTRL "+R", this, SLOT(refresh())),
              *clearButton = ui->makeToolButton(AcUi::PushHint, TR(T_CLEAR), tr("Clear library"), this, SLOT(clear()));

  QToolButton *hideButton = ui->makeToolButton(0, "[X]", tr("Hide view"), this, SLOT(hide())),
              *toggleButton = ui->makeToolButton(0, "[-]", tr("Toggle view"), this, SIGNAL(toggled()));
  hideButton->setStyleSheet(SS_HEADER_BUTTON);
  toggleButton->setStyleSheet(SS_HEADER_BUTTON);
  hideButton->setGraphicsEffect(ui->makeHaloEffect(Qt::yellow));
  toggleButton->setGraphicsEffect(ui->makeHaloEffect(Qt::yellow));

  new QShortcut(QKeySequence("CTRL+R"), refreshButton, SLOT(click()));

  iconButton_ = ui->makeRadioButton(AcUi::DefaultHint, QString(), tr("Icon View"), K_CTRL "+1"),
  listButton_ = ui->makeRadioButton(AcUi::DefaultHint, QString(), tr("List View"), K_CTRL "+2");
  iconButton_->setChecked(true);

  connect(iconButton_, SIGNAL(clicked()), SLOT(showIcon()));
  connect(listButton_, SIGNAL(clicked()), SLOT(showList()));

  new QShortcut(QKeySequence("CTRL+1"), iconButton_, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+2"), listButton_, SLOT(click()));
//#ifndef Q_OS_MAC
//  new QShortcut(QKeySequence("ALT+1"), iconButton_, SLOT(click()));
//  new QShortcut(QKeySequence("ALT+2"), listButton_, SLOT(click()));
//#endif // Q_OS_MAC

  // Create layout

  views_ = new QStackedLayout;
  views_->addWidget(iconView_);
  views_->addWidget(listView_);

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(views_);
    rows->addLayout(footer);

    //header->addWidget(refreshButton);
    //header->addWidget(clearButton);
    //header->addStretch();

    header->addWidget(hideButton);
    header->addWidget(toggleButton);
    header->addWidget(iconButton_);
    header->addWidget(listButton_);
    header->addStretch();
    header->addWidget(filterType_);

    QxOverlayLayout *overlay = new QxOverlayLayout;
    overlay->addWidget(filterEdit_);
    overlay->addWidget(countButton_, Qt::AlignRight);
    header->addLayout(overlay);

    footer->addWidget(openButton_);
    footer->addWidget(browseButton_);
    footer->addStretch();
    footer->addWidget(clearButton);
    footer->addWidget(refreshButton);

    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;
    rows->setContentsMargins(patch, patch, patch, patch);
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    views_->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }  setCentralWidget(new QxLayoutWidget(rows, this));
}

// - Properties -

int
MediaLibraryView::viewIndex() const
{ return views_->currentIndex(); }

void
MediaLibraryView::setViewIndex(int index)
{
  switch (index) {
  case IconView: iconButton_->click(); break;
  case ListView: listButton_->click(); break;
  }
}

bool
MediaLibraryView::autoHide() const
{ return autoHideAct_->isChecked(); }

bool
MediaLibraryView::autoRun() const
{ return autoRunAct_->isChecked(); }

void
MediaLibraryView::setAutoHide(bool t)
{ autoHideAct_->setChecked(t); }

void
MediaLibraryView::setAutoRun(bool t)
{
  if (t != autoRun()) {
    autoRunAct_->setChecked(t);
    emit autoRunChanged(t);
  }
}

// - Actions -

void
MediaLibraryView::showIcon()
{ views_->setCurrentWidget(iconView_); }

void
MediaLibraryView::showList()
{ views_->setCurrentWidget(listView_); }

void
MediaLibraryView::refresh()
{
  //showMessage(tr("refreshing library"));
  library_->refresh();
  updateCount();
  updateButtons();
  //listView_->setColumnWidth(0, 350);
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
MediaLibraryView::showGame()
{
  if (!selectionModel_->hasSelection())
    return;
  QModelIndex index = selectionModel_->currentIndex();
  int type  = MediaModel::indexType(index);
  if (type != Media::Game)
    return;
  QString digest = MediaModel::indexDigest(index);
  if (digest.isEmpty())
    return;
  emit showGameRequested(digest);
}

void
MediaLibraryView::open()
{
  if (!selectionModel_->hasSelection())
    return;

  QModelIndex index = selectionModel_->currentIndex();
  QString location = MediaModel::indexLocation(index);
  if (location.isEmpty())
    return;

  if (!location.contains("://") && !QFile::exists(location)) {
    warn(tr("not exist") + ": " + location);
    return;
  }

  showMessage(tr("opening") + ": " + location);
  emit openRequested(location);
  if (autoHide())
    fadeOut();
}

void
MediaLibraryView::showSelection()
{
  if (selectionModel_->hasSelection())  {
    QModelIndex index = selectionModel_->currentIndex();
    QString location = MediaModel::indexLocation(index);
    if (!location.isEmpty())
      showMessage(location);
  }
}

void
MediaLibraryView::browse()
{
  if (!selectionModel_->hasSelection())
    return;

  QModelIndex index = selectionModel_->currentIndex();
  QString location = MediaModel::indexLocation(index);
  if (location.isEmpty())
    return;

  QString url;
  if (location.contains("://"))
    url = location;
  else {
    QFileInfo fi(location);
    if (!fi.isDir())
      fi = QFileInfo(fi.absolutePath());
    url = fi.absoluteFilePath();
    if (!fi.exists()) {
      warn(tr("not exist") + ": " + url);
      return;
    }
  }
  showMessage(tr("opening") + ": " + url);
  if (!url.contains("://")) {
#ifdef Q_OS_WIN
    url = QDir::fromNativeSeparators(url);
    url.prepend("file:///");
#else
    url.prepend("file://");
#endif // Q_OS_WIN
  }
  QDesktopServices::openUrl(url);
}

// - Filters -

void
MediaLibraryView::updateFilterEdit()
{
  QString rx = filterEdit_->text().trimmed();
  if (rx.isEmpty())
    proxyModel_->setFilterRegExp(QString());
  else {
    rx.replace(QRegExp("\\s+"), ".*");
    //QStringList l = rx.split(' ', QString::SkipEmptyParts);
    //if (l.size() > 1) {
    //  rx = "(?:";
    //  foreach (const QString &t, l)
    //    rx.append("(?:")
    //      .append(t)
    //      .append(")");
    //  rx.append(")+");
    //}
    proxyModel_->setFilterRegExp(QRegExp(rx, Qt::CaseInsensitive));
  }
  updateCount();
  if (!rx.isEmpty())
    showMessage(tr("regex") + ": " + rx);
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
  case FT_Folder: key = TR(T_FOLDER); break;
  case FT_Video:  key = TR(T_VIDEO); break;
  case FT_Url:    key = TR(T_URL); break;
#ifdef AC_ENABLE_GAME
  case FT_Game:   key = TR(T_GALGAME); break;
#endif // AC_ENABLE_GAME
  }
  filterModel_->setFilterFixedString(key);
  updateCount();
}

// - Events -

void
MediaLibraryView::updateButtons()
{
  bool t = selectionModel_->hasSelection();
  openButton_->setEnabled(t);
  browseButton_->setEnabled(t);
}

void
MediaLibraryView::updateCount()
{
  int total = library_->model()->rowCount(),
      count = proxyModel_->rowCount();
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
  if (visible)
    focus();
  Base::setVisible(visible);
  //iconView_->setResizeMode(visible ? QListView::Adjust : QListView::Fixed);
}

void
MediaLibraryView::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  updateContextMenu();
  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
MediaLibraryView::syncGame()
{
  showMessage(tr("searching for running game ..."));
  emit syncGameRequested();
}

void
MediaLibraryView::updateContextMenu()
{
  bool v = selectionModel_->hasSelection();
  openAct_->setVisible(v);
  browseAct_->setVisible(v);

#ifdef AC_ENABLE_GAME
  syncGameAct_->setVisible(library_->hasGames());
  if (v && library_->hasGames()) {
    QModelIndex index = selectionModel_->currentIndex();
    int type  = MediaModel::indexType(index);
    showGameAct_->setVisible(type == Media::Game);
  } else
    showGameAct_->setVisible(false);
#endif // AC_ENABLE_GAME
}

// EOF
