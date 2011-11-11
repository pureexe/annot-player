// processview.cc
// 10/16/2011

#include "processview.h"
#include "lineedit.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "global.h"
#include "logger.h"
#ifdef USE_WIN_QTWIN
  #include "win/qtwin/qtwin.h"
#else
  #error "Currently only implemented in windows."
#endif // USE_WIN_QTWIN
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#else
  #error "QTH is indispensible"
#endif // USE_WIN_QTH
#include "core/gui/combobox.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

using namespace Logger;

// - Helpers -

namespace { // anonymous

  bool processNameIsNotGalGame(const QString &procName)
  {
    static QStringList blacklist;
    if (blacklist.isEmpty()) {
#define ADD(_app)       blacklist.append(_app)
      ADD("Activator");
      ADD("ApMsgFwd");
      ADD("Apntex");
      ADD("Apoint");
      ADD("AutoHotkey");
      ADD("BoonSutazio");
      ADD("Bootcamp");
      ADD("BtStackServer");
      ADD("BTTray");
      ADD("chrome");
      ADD("Dropbox");
      ADD("DTLite");
      ADD("eclipse");
      ADD("Evernote");
      ADD("EvernoteTray");
      ADD("firefox");
      ADD("foobar2000");
      ADD("GoogleIMEJaConverter");
      ADD("GoogleIMEJaRenderer");
      ADD("gvim");
      ADD("Hamana");
      ADD("HidFind");
      ADD("IELowutil");
      ADD("IEXPLOR");
      ADD("iTunes");
      ADD("iTunesHelper");
      ADD("java");
      ADD("javaw");
      ADD("KHALMNPR");
      ADD("KMPlayer");
      ADD("MacDrive");
      ADD("Maxthon");
      ADD("MouseGesture");
      ADD("mspdbsrv");
      ADD("mysql");
      ADD("oacrmonitor");
      ADD("ONENOTEM");
      ADD("opera");
      ADD("php-cgi");
      ADD("QQ");
      ADD("qtcreator");
      ADD("SecureCRT");
      ADD("SetPoint");
      ADD("softinfo");
      ADD("SogouCloud");
      ADD("ssh");
      ADD("sttray");
      ADD("Switcher");
      ADD("thunderbird");
      ADD("TXPlatform");
      ADD("vim");
      ADD("volumouse");
#undef ADD
    }
    return blacklist.indexOf(procName) >= 0;
  }

} // anonymous namespace

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

ProcessView::ProcessView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(tr("Process view"));
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();
  createActions();

  // Set initial states
  proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  filterColumnComboBox_->setCurrentIndex(HD_Name);

  filterPatternLineEdit_->setFocus();
}

void
ProcessView::createLayout()
{
  // Create models

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setProcessHeaderData(sourceModel_);

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
    proxyView_->setToolTip(tr("Running processes"));
  }

  filterPatternLineEdit_ = new LineEdit; {
    filterPatternLineEdit_->setStyleSheet(SS_LINEEDIT);
    filterPatternLineEdit_->setToolTip(tr("Filter pattern"));
  }
  QLabel *filterPatternLabel = new QLabel; {
    filterPatternLabel->setStyleSheet(SS_LABEL);
    filterPatternLabel->setBuddy(filterPatternLineEdit_);
    filterPatternLabel->setText(tr("&Filter pattern") + ":");
    filterPatternLabel->setToolTip(filterPatternLineEdit_->toolTip());
  }

  filterSyntaxComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterSyntaxComboBox_);
    filterSyntaxComboBox_->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox_->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox_->addItem(tr("Fixed string"), QRegExp::FixedString);
    filterSyntaxComboBox_->setToolTip(tr("Filter syntax"));
  }
  QLabel *filterSyntaxLabel = new QLabel; {
    filterSyntaxLabel->setStyleSheet(SS_LABEL);
    filterSyntaxLabel->setBuddy(filterSyntaxComboBox_);
    filterSyntaxLabel->setText(tr("Filter &syntax") + ":");
    filterSyntaxLabel->setToolTip(filterSyntaxComboBox_->toolTip());
  }

  filterColumnComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterColumnComboBox_);
    for (int i = 0; i < sourceModel_->columnCount(); i++)
      filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
    filterColumnComboBox_->setToolTip(tr("Filter column"));
  }
  QLabel *filterColumnLabel = new QLabel; {
    filterColumnLabel->setStyleSheet(SS_LABEL);
    filterColumnLabel->setBuddy(filterColumnComboBox_);
    filterColumnLabel->setText(tr("Filter &column"));
    filterColumnLabel->setToolTip(filterColumnComboBox_->toolTip());
  }

  /*
#define MAKE_TOKEN_LABEL(_id, _styleid) \
  _id##Label_ = new QLabel; { \
    _id##Label_->setStyleSheet(SS_LABEL); \
    _id##Label_->setText(TR(T_UNKNOWN)); \
    _id##Label_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  } \
  QLabel *_id##Buddy = new QLabel; { \
    _id##Buddy->setStyleSheet(SS_LABEL); \
    _id##Buddy->setBuddy(_id##Label_); \
    _id##Buddy->setText(TR(T_LABEL_##_styleid)); \
    _id##Buddy->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  }

  MAKE_TOKEN_LABEL(createDate, CREATEDATE)
  MAKE_TOKEN_LABEL(blessedCount, BLESSEDCOUNT)
  MAKE_TOKEN_LABEL(cursedCount, CURSEDCOUNT)
  MAKE_TOKEN_LABEL(visitedCount, VISITEDCOUNT)
#undef MAKE_TOKEN_LABEL

  QLabel *aliasBuddy = new QLabel; {
    aliasBuddy->setStyleSheet(SS_LABEL);
    aliasBuddy->setBuddy(addAliasButton);
    aliasBuddy->setText(TR(T_LABEL_ALIAS));
    aliasBuddy->setToolTip(TR(T_TOOLTIP_ALIAS));
  }
  */

#define MAKE_BUTTON(_button, _text, _tip, _slot) \
  _button = new Core::Gui::ToolButton; { \
    _button->setText(QString("[ %1 ]").arg(_text)); \
    _button->setToolTip(_tip); \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
  } \
  connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(attachButton_, TR(T_ATTACH), TR(T_TOOLTIP_ATTACHPROCESS), SLOT(attachProcess()))
  MAKE_BUTTON(detachButton_, TR(T_DETACH), TR(T_TOOLTIP_DETACHPROCESS), SLOT(detachProcess()))

  QToolButton *MAKE_BUTTON(refreshButton, TR(T_REFRESH), TR(T_TOOLTIP_REFRESHPROCESS), SLOT(refresh()))
#undef MAKE_BUTTON

  // Set layout

  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;

    layout->addWidget(attachButton_, r=0, c=0);
    layout->addWidget(detachButton_, r, ++c);
    layout->addWidget(refreshButton, r, ++c);

    layout->addWidget(proxyView_, ++r, c=0, 1, 3);

    layout->addWidget(filterPatternLabel, ++r, c=0);
    layout->addWidget(filterPatternLineEdit_, r, ++c, 1, 2);

    layout->addWidget(filterSyntaxLabel, ++r, c=0);
    layout->addWidget(filterSyntaxComboBox_, r, ++c, 1, 2);

    layout->addWidget(filterColumnLabel, ++r, c=0);
    layout->addWidget(filterColumnComboBox_, r, ++c, 1, 2);

    //layout->setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  //QVBoxLayout *rows = new QVBoxLayout; {
  //  // (row, col, rowspan, colspan, alignment)
  //  QHBoxLayout *header = new QHBoxLayout,
  //              *filterPatternRow = new QHBoxLayout,
  //              *filterSyntaxRow = new QHBoxLayout,
  //              *filterColumnRow = new QHBoxLayout;
  //  rows->addLayout(header);
  //  rows->addWidget(proxyView_);
  //  rows->addLayout(filterPatternRow);
  //  rows->addLayout(filterSyntaxRow);
  //  rows->addLayout(filterColumnRow);
  //
  //  header->addWidget(attachButton_);
  //  header->addWidget(detachButton_);
  //  header->addStretch();
  //  header->addWidget(refreshButton);
  //
  //  filterPatternRow->addWidget(filterPatternLabel);
  //  filterPatternRow->addWidget(filterPatternLineEdit_);
  //
  //  filterSyntaxRow->addWidget(filterSyntaxLabel);
  //  filterSyntaxRow->addWidget(filterSyntaxComboBox_);
  //  filterSyntaxRow->addStretch();
  //
  //  filterColumnRow->addWidget(filterColumnLabel);
  //  filterColumnRow->addWidget(filterColumnComboBox_);
  //  filterColumnRow->addStretch();
  //
  //  //layout->setContentsMargins(0, 0, 0, 0);
  //}
  //setLayout(rows);

  // Set up connections
  connect(filterPatternLineEdit_, SIGNAL(textChanged(QString)),
          SLOT(invalidateFilterRegExp()));
  connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterRegExp()));
  connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)),
          SLOT(invalidateButtons()));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)),
          SLOT(invalidateButtons()));

}

void
ProcessView::setProcessHeaderData(QAbstractItemModel *model)
{
  model->setHeaderData(HD_Name, Qt::Horizontal, tr("Name"));
  model->setHeaderData(HD_Pid, Qt::Horizontal, tr("Pid"));
  model->setHeaderData(HD_Status, Qt::Horizontal, tr("Status"));
  model->setHeaderData(HD_Folder, Qt::Horizontal, tr("Folder"));
  model->setHeaderData(HD_Path, Qt::Horizontal, tr("Path"));
}

void
ProcessView::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setStatusTip(TR(T_STATUSTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(attachAct_,  ATTACHPROCESS, SLOT(detachProcess()))
  MAKE_ACTION(detachAct_,  DETACHPROCESS, SLOT(detachProcess()))
  MAKE_ACTION(refreshAct_, REFRESHPROCESS, SLOT(refresh()))

#undef MAKE_ACTION

  // Create menus
  contextMenu_ = new QMenu(windowTitle(), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
}


// - Properties -

QModelIndex
ProcessView::currentIndex() const
{ return proxyView_->currentIndex(); }

ulong
ProcessView::currentPid() const
{
  QModelIndex mi = currentIndex();
  if (!mi.isValid())
    return 0;
  mi = mi.sibling(mi.row(), HD_Pid);
  if (!mi.isValid())
    return 0;
  bool ok;
  ulong ret = mi.data().toUInt(&ok);
  if (!ok)
    return 0;
  return ret;
}

ulong
ProcessView::attachedPid() const
{
  QList<QStandardItem*> items =
      sourceModel_->findItems(TR(T_ATTACHED), Qt::MatchExactly, HD_Status);
  if (items.empty())
    return 0;

  QStandardItem *item = items.front();
  item = sourceModel_->item(item->row(), HD_Pid);
  if (!item)
    return 0;
  bool ok;
  ulong ret = item->data(Qt::EditRole).toUInt(&ok);
  if (!ok)
    return 0;
  return ret;
}

ProcessInfo
ProcessView::attachedProcessInfo() const
{ return pis_[attachedPid()]; }

// - Slots -

void
ProcessView::refresh()
{
  log(tr("refreshing process list ..."));
  invalidateSourceModel();
  invalidateButtons();
}

void
ProcessView::invalidateSourceModel(bool showAll)
{
  //using QtWin::ProcessInfo;
  static QString windir = QtWin::getWinDir();

  clear();

  foreach (QtWin::ProcessInfo wpi, QtWin::getProcessesInfo()) {
    ulong pid = wpi.id;
    QString name;
    QString folder;
    QString filePath;
    if (showAll) {
      name = wpi.name;
      filePath = QtWin::getProcessPathById(pid);
      if (!filePath.isEmpty())
        folder = QFileInfo(filePath).dir().dirName();
    } else {
      if (pid == QCoreApplication::applicationPid())
        continue;
      if (wpi.name.isEmpty())
        continue;

      filePath = QtWin::getProcessPathById(pid);
      if (filePath.isEmpty())
        continue;
      if (filePath.startsWith(windir, Qt::CaseInsensitive))
        continue;

      QFileInfo fi(filePath);
      if (!fi.exists())
        continue;

      name = fi.baseName();
      if (::processNameIsNotGalGame(name))
        continue;

      folder = fi.dir().dirName();
    }

    QString status;
    if (QTH->isProcessAttached(pid))
      status = TR(T_ATTACHED);

    // TODO: hidden PID: pi.pid save as row attribute but hidden
    ProcessInfo pi;
    pi.processId = pid;
    pi.processName = name;
    pi.executablePath = filePath;
    pis_[pid] = pi;

    sourceModel_->insertRow(0);
    sourceModel_->setData(sourceModel_->index(0, HD_Name), name);
    sourceModel_->setData(sourceModel_->index(0, HD_Pid), (quint32)pid); // quint32: potential problem when porting to 64bit machine
    sourceModel_->setData(sourceModel_->index(0, HD_Status), status);
    sourceModel_->setData(sourceModel_->index(0, HD_Folder), folder);
    sourceModel_->setData(sourceModel_->index(0, HD_Path), filePath);

    for (int i = 0; i < HD_Count; i++)
      sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);
  }
}

void
ProcessView::clear()
{
  //if (sourceModel_->rowCount() == 0)
  //  return;
  //sourceModel_->clear();
  //setProcessHeaderData(sourceModel_);

  pis_.clear();

  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
}

// - Slots -

void
ProcessView::invalidateFilterRegExp()
{
  QRegExp::PatternSyntax syntax =
      QRegExp::PatternSyntax(
        filterSyntaxComboBox_->itemData(
          filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternLineEdit_->text(), Qt::CaseInsensitive, syntax);
  proxyModel_->setFilterRegExp(regExp);
}

void
ProcessView::invalidateFilterColumn()
{ proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex()); }

void
ProcessView::invalidateButtons()
{
  ulong cur = currentPid();
  if (!cur) {
    attachButton_->setEnabled(false);
    detachButton_->setEnabled(false);
    return;
  }

  ulong a = attachedPid();

  attachButton_->setEnabled(!a);
  detachButton_->setEnabled(a && a == cur);
}

void
ProcessView::attachProcess()
{
  ulong pid = currentPid();
  if (pid && QTH->attachProcess(pid)) {
    setCurrentItemAttached(true);
    invalidateButtons();
    log(tr("process attached") + QString(" (pid = %1)").arg(QString::number(pid)));
  } else {
    error(tr("failed to attach process ") + QString(" (pid = %1)").arg(QString::number(pid)));
    if (!QtWin::isProcessActiveWithId(pid))
      notify(tr("Is the process running now?"));
    else if (!QTH->isElevated())
      notify(tr("Run me as administrator and try again &gt;_&lt;"));
    else
      notify(tr("Restart the target process might help -_-"));
  }
}

void
ProcessView::detachProcess()
{
  ulong pid = currentPid();
  if (pid)
    setCurrentItemAttached(false);
  if (pid && QTH->detachProcess(pid)) {
    invalidateButtons();
    log(tr("process detached") + QString(" (pid = %1)").arg(QString::number(pid)));
  } else
    warn(tr("failed to detach process") + QString(" (pid = %1)").arg(QString::number(pid)));
}

// - Events -

void ProcessView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void ProcessView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void ProcessView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void ProcessView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

void
ProcessView::setCurrentItemAttached(bool attached)
{
  QModelIndex mi = currentIndex();
  if (!mi.isValid())
    return;
  mi = mi.sibling(mi.row(), HD_Status);
  if (!mi.isValid())
    return;
  QString value;
  if (attached)
    value = TR(T_ATTACHED);

  proxyModel_->setData(mi, value);
}

void
ProcessView::setVisible(bool t)
{
  if (t)
    refresh();
  Base::setVisible(t);
}

void
ProcessView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  contextMenu_->clear();

  if (currentIndex().isValid()) {

    if (attachButton_->isEnabled())
      contextMenu_->addAction(attachAct_);

    if (detachButton_->isEnabled())
      contextMenu_->addAction(detachAct_);

    contextMenu_->addSeparator();
  }

  contextMenu_->addAction(refreshAct_);

  contextMenu_->popup(event->globalPos());
}

// EOF
