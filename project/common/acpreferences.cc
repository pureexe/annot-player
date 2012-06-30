// acpreferences.cc
// 5/5/2012

#include "project/common/acpreferences.h"
#include "project/common/acpreferencestab.h"
#include "project/common/acaccountprefs_p.h"
#include "project/common/aclocationprefs_p.h"
#include "project/common/acnetworkproxyprefs_p.h"
#include "project/common/acsettings.h"
#include "project/common/actabview.h"
#include "module/qtext/layoutwidget.h"
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowTitleHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS  Qt::FramelessWindowHint | WINDOW_FLAGS_BASE
#else
# define WINDOW_FLAGS  WINDOW_FLAGS_BASE
#endif // Q_OS_MAC

// - Constructions -

AcPreferences::AcPreferences(QWidget *parent, bool final)
  : Base(parent, WINDOW_FLAGS), settings_(0), tabView_(0), tabs_(0)
{
  init();
  if (final)
    finalize();
}

AcPreferences::AcPreferences(ulong tabs, QWidget *parent, bool final)
  : Base(parent, WINDOW_FLAGS), settings_(0), tabView_(0), tabs_(tabs)
{
  init();
  if (final)
    finalize();
}

void
AcPreferences::init()
{
  tabView_ = new AcTabView(this);
  if (!settings_)
    settings_ = AcSettings::globalSettings();
  setWindowTitle(tr("Preferences"));
}

void
AcPreferences::finalize()
{
  createLayout();

  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));
}

void
AcPreferences::addTab(AcPreferencesTab *tab)
{
  connect(tab, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(tab, SIGNAL(error(QString)), SLOT(error(QString)));
  connect(tab, SIGNAL(warning(QString)), SLOT(warn(QString)));
  connect(tab, SIGNAL(notification(QString)), SLOT(notify(QString)));
  tabView_->addTab(tab);
}

void
AcPreferences::createLayout()
{
  if (!tabs_ || tabs_ & LocationTab)
    addTab(new AcLocationPrefs(settings_, this));
  if (!tabs_ || tabs_ & AccountTab)
    addTab(new AcAccountPrefs(settings_, this));
  if (!tabs_ || tabs_ & NetworkProxyTab)
    addTab(new AcNetworkProxyPrefs(settings_, this));
  tabView_->finalizeLayout();

  setCentralWidget(tabView_);
  connect(tabView_, SIGNAL(tabChanged(int)), SLOT(save()));
  connect(tabView_, SIGNAL(tabChanged(int)), SLOT(updateSize()));

  //AcUi *ui = AcUi::globalInstance();
  //QToolButton *okButton = ui->makeToolButton(
  //      AcUi::PushHint | AcUi::HighlightHint, tr("Save"), this, SLOT(save()));
  //QToolButton *cancelButton = ui->makeToolButton(
  //      AcUi::PushHint, tr("Cancel"), this, SLOT(fadeOut()));

  // Layout
  //QVBoxLayout *rows = new QVBoxLayout; {
  //  QHBoxLayout *footer = new QHBoxLayout;
  //  rows->addWidget(tabView);
  //  rows->addLayout(footer);
//
  //  footer->addWidget(cancelButton);
  //  footer->addWidget(okButton);
//
  //  // left, top, right, bottom
  //  int patch = 0;
  //  if (!AcUi::isAeroAvailable())
  //    patch = 9;
  //  footer->setContentsMargins(0, 0, 0, 0);
  //  rows->setContentsMargins(9, patch, 9, 9);
  //  setContentsMargins(0, 0, 0, 0);
  //} setCentralWidget(new LayoutWidget(rows));
}

// - Properties -

QSize
AcPreferences::sizeHint() const
{
  enum { margin = 9 };
  QSize ret = centralWidget()->sizeHint();
  ret.rheight() += statusBar()->sizeHint().height();
  return ret + QSize(margin, margin);
}

void
AcPreferences::updateSize()
{ setFixedSize(sizeHint()); }

// - Events -

void
AcPreferences::setVisible(bool visible)
{
  if (visible != isVisible()) {
    if (visible) {
      load();
      updateSize();
    } else {
      save();
      clear();
    }
  }
  Base::setVisible(visible);
}

// - Actions -

void
AcPreferences::save()
{
  bool ok = true;
  QWidget *current = tabView_->currentWidget();
  foreach (QWidget *t, tabView_->widgets())
    if (t != current)
      ok = qobject_cast<AcPreferencesTab *>(t)->save()
         && ok;

  // Save current at last, so that its status messages are preserved
  ok = qobject_cast<AcPreferencesTab *>(current)->save()
     && ok;

  if (ok)
    settings_->sync();
}

void
AcPreferences::load()
{
  //settings_->sync();
  foreach (QWidget *t, tabView_->widgets())
    qobject_cast<AcPreferencesTab *>(t)->load();
}

void
AcPreferences::clear()
{
  foreach (QWidget *t, tabView_->widgets())
    qobject_cast<AcPreferencesTab *>(t)->clear();
}

// EOF
