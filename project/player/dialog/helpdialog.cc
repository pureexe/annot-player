// helpdialog.cc
// 11/16/2011

#include "helpdialog.h"
#include "global.h"
#include "tr.h"
#include "ac/acsettings.h"
#include <QtGui>
#include <QtWebKit>

// - Construction -

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

HelpDialog::HelpDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_HELP));
  resize(800, 600);

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence("CTRL+W"), this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
}

// - Properties -

QUrl
HelpDialog::urlForLanguage(QLocale::Language lang)
{
  switch (lang) {
  case QLocale::Japanese: return QString(G_HELPPAGE_URL "/ja");
  case QLocale::Chinese:  return QString(G_HELPPAGE_URL "/zh");
  case QLocale::English:  return QString(G_HELPPAGE_URL "/en");
  default: return QString(G_HELPPAGE_URL);
  }
}

// - Events -

void
HelpDialog::setVisible(bool visible)
{
  if (visible) {
    QUrl url = urlForLanguage(
      (QLocale::Language)AcSettings::globalSettings()->language()
    );
    webView()->load(url);
  }
  Base::setVisible(visible);
}

// EOF
