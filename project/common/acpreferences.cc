// acpreferences.cc
// 5/5/2012

#include "project/common/acpreferences.h"
#include "project/common/acpreferences_p.h"
#include "project/common/acsettings.h"
#include "project/common/acui.h"
#include "project/common/actabview.h"
#include "module/qtext/layoutwidget.h"
#include <QtGui>

// - Constructions -

void
AcPreferences::init()
{
  if (!settings_)
    settings_ = AcSettings::globalSettings();
  setWindowTitle(tr("Preferences"));
  createLayout();
}

void
AcPreferences::addTab(AcPreferencesTab *tab)
{
  connect(tab, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(tab, SIGNAL(error(QString)), SLOT(error(QString)));
  connect(tab, SIGNAL(warning(QString)), SLOT(warn(QString)));
  connect(tab, SIGNAL(notification(QString)), SLOT(notify(QString)));
  tabs_.append(tab);
}

void
AcPreferences::createLayout()
{
  addTab(new AcLocationPreferences(settings_));
  addTab(new AcEmptyPreferences(settings_));

  AcUi *ui = AcUi::globalInstance();
  AcTabView *tabView = new AcTabView;
  foreach (AcPreferencesTab *tab, tabs_)
    tabView->addTab(tab);
  tabView->finalizeLayout();

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("OK"), this, SLOT(save()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, tr("Cancel"), this, SLOT(hide()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(tabView);
    rows->addLayout(footer);

    footer->addWidget(cancelButton);
    footer->addWidget(okButton);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, patch, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new LayoutWidget(rows));
}

// - Events -

void
AcPreferences::setVisible(bool visible)
{
  if (visible != isVisible())
    refresh();
  Base::setVisible(visible);
}

// - Actions -

void
AcPreferences::save()
{
  foreach (AcPreferencesTab *tab, tabs_)
    tab->save();
  settings_->sync();

  hide();
}

void
AcPreferences::refresh()
{
  //settings_->sync();
  foreach (AcPreferencesTab *tab, tabs_)
    tab->load();
}

// EOF
