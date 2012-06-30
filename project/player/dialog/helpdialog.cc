// helpdialog.cc
// 11/16/2011

#include "helpdialog.h"
#include "global.h"
#include "tr.h"
#include "project/common/acsettings.h"
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

  webView()->setRenderHints(
    //QPainter::Antialiasing |
    QPainter::TextAntialiasing
    //QPainter::HighQualityAntialiasing |
    //QPainter::SmoothPixmapTransform
  );

  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
}

// - Properties -

QUrl
HelpDialog::urlForLanguage(int lang)
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
    QUrl url = urlForLanguage(AcSettings::globalSettings()->language());
    webView()->load(url);
  }
  Base::setVisible(visible);
}

// EOF
