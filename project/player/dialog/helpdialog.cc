// helpdialog.cc
// 11/16/2011

#include "helpdialog.h"
#include "global.h"
#include "tr.h"
#include "settings.h"

#include <QtGui>
#include <QtWebKit>

#define TEXT_SIZE_SCALE 0.85

// - Constructions -
HelpDialog::HelpDialog(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(TR(T_TITLE_HELP));
  setTextSizeMultiplier(TEXT_SIZE_SCALE);
  resize(INIT_WINDOW_SIZE);
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
      (QLocale::Language)Settings::globalInstance()->language()
    );
    load(url);
  }
  Base::setVisible(visible);
}

// EOF
