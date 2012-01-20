// processview.cc
// 10/16/2011

#include "processview.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "defines.h"
#include "logger.h"
#include "filteredtableview.h"
#include "win/qtwin/qtwin.h"
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#else
  #error "QTH is indispensible"
#endif // USE_WIN_QTH
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
      ADD("Activator"); ADD("ApMsgFwd"); ADD("Apntex"); ADD("Apoint"); ADD("APSDaemon"); ADD("AutoHotkey"); ADD("ApplePhotoStreams");
      ADD("BookmarkDAV_client"); ADD("BoonSutazio"); ADD("Bootcamp"); ADD("BtStackServer"); ADD("BTTray");
      ADD("CamtasiaStudio"); ADD("chrome");
      ADD("Dropbox"); ADD("DTLite");
      ADD("eclipse"); ADD("Evernote"); ADD("EvernoteTray");
      ADD("firefox"); ADD("foobar2000");
      ADD("GoogleIMEJaConverter"); ADD("GoogleIMEJaRenderer"); ADD("gvim");
      ADD("Hamana"); ADD("HidFind");
      ADD("iCloudServices"); ADD("IELowutil"); ADD("IEXPLOR"); ADD("iTunes"); ADD("iTunesHelper");
      ADD("java"); ADD("javaw");
      ADD("KHALMNPR"); ADD("KMPlayer");
      ADD("MacDrive"); ADD("Maxthon"); ADD("MouseGesture"); ADD("mspdbsrv"); ADD("mysql");
      ADD("netdrive");
      ADD("oacrmonitor"); ADD("ONENOTEM"); ADD("opera");
      ADD("php-cgi");
      ADD("QQ"); ADD("qtcreator");
      ADD("SecureCRT"); ADD("SetPoint"); ADD("sidebar"); ADD("softinfo"); ADD("SogouCloud"); ADD("sttray"); ADD("Switcher");
      ADD("thunderbird"); ADD("TSCHelper"); ADD("TXPlatform");
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

  createModel();
  createLayout();
  createActions();

  // Set initial states
  tableView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  tableView_->setCurrentColumn(HD_Name);
}

void
ProcessView::createModel()
{
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setProcessHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new FilteredTableView(sourceModel_, proxyModel_, this);
  connect(tableView_, SIGNAL(currentIndexChanged(QModelIndex)), SLOT(invalidateButtons()));
}

void
ProcessView::createLayout()
{
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

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    header->addWidget(attachButton_);
    header->addWidget(detachButton_);
    header->addStretch();
    header->addWidget(refreshButton);

    rows->addLayout(header);
    rows->addWidget(tableView_);

    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);
}

void
ProcessView::setProcessHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Name, Qt::Horizontal,TR(T_NAME));
  model->setHeaderData(HD_Pid, Qt::Horizontal, TR(T_PID));
  model->setHeaderData(HD_Status, Qt::Horizontal, TR(T_STATUS));
  model->setHeaderData(HD_Folder, Qt::Horizontal, TR(T_FOLDER));
  model->setHeaderData(HD_Path, Qt::Horizontal, TR(T_PATH));
}

void
ProcessView::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
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
{ return tableView_->currentIndex(); }

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
  static QString windir = QtWin::getWinDirPath();

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
  pis_.clear();
  tableView_->clear();
}

// - Slots -

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
    log(QString("%1 (pid = %2)").arg(tr("process attached")).arg(QString::number(pid)));
    emit attached(pis_[pid]);
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
  ProcessInfo pi;
  if (pid) {
    pi = pis_[pid];
    setCurrentItemAttached(false);
  }
  if (pid && QTH->detachProcess(pid)) {
    invalidateButtons();
    log(tr("process detached") + QString(" (pid = %1)").arg(QString::number(pid)));
  } else
    warn(tr("failed to detach process") + QString(" (pid = %1)").arg(QString::number(pid)));
  emit detached(pi);
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
