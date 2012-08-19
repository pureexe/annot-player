// acabout.cc
// 11/13/2011

#include "project/common/acabout.h"
#include "project/common/acglobal.h"
#include "project/common/acui.h"
#include "project/common/acupdater.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
# define WINDOW_FLAGS ( \
    Qt::WindowTitleHint | \
    WINDOW_FLAGS_BASE )
#endif // Q_OS_MAC

// - Constructions -

#ifdef Q_OS_MAC
# define WINDOW_SIZE   QSize(300, 200)
#else
# define WINDOW_SIZE   QSize(270, 160)
#endif // Q_OS_MAC

QString
AcAbout::templateText() const
{
  static const char *ret =
    HTML_CENTER_OPEN() \
      HTML_SS_OPEN(color:purple) \
        HTML_BR() \
\
        AC_ORGANIZATION " " \
        "%1" " " \
        "%2" \
        HTML_BR() \
\
        HTML_EM_OPEN() \
          AC_LICENSE \
        HTML_EM_CLOSE() \
        HTML_BR() \
\
        HTML_BR() \
\
        HTML_A_OPEN("dummy") \
          "http://annot.me" HTML_BR() \
          AC_UPDATEPAGE     HTML_BR() \
          AC_EMAIL          HTML_BR() \
        HTML_A_CLOSE() \
\
      HTML_SS_CLOSE() \
    HTML_CENTER_CLOSE()
  ;
  return ret;
}

void
AcAbout::init()
{
  setRippleEnabled(true);
  setWindowTitle(tr("About"));
  createLayout();
}

void
AcAbout::createLayout()
{
  AcUi::globalInstance()->setWindowStyle(this);

  // Components

  QToolButton *okButton = AcUi::globalInstance()->makeToolButton(
       AcUi::PushHint | AcUi::HighlightHint, tr("OK"), this, SLOT(fadeOut()));
  QToolButton *updateButton = AcUi::globalInstance()->makeToolButton(
       AcUi::PushHint, tr("Update"), this, SLOT(showUpdater()));

#ifdef AC_ENABLE_UPDATE
  connect(updateButton, SIGNAL(clicked()), SLOT(fadeOut()));
#endif // AC_ENABLE_UPDATE

  textEdit_ = AcUi::globalInstance()->makeTextEdit(
      AcUi::ReadOnlyHint, tr("About"));

  // Layout

  QVBoxLayout *rows = new QVBoxLayout; {

    QHBoxLayout *footer = new QHBoxLayout;

    rows->addWidget(textEdit_);
    rows->addLayout(footer);

    footer->addWidget(updateButton);
    footer->addWidget(okButton);

    // l, t, r, b
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;
    rows->setContentsMargins(patch, patch, patch, 0);
    setContentsMargins(4, 4, 4, patch);
  } setLayout(rows);

  resize(WINDOW_SIZE);

  // Focus
  okButton->setFocus();
}

void
AcAbout::setVisible(bool visible)
{
  if (visible)
    textEdit_->setHtml(text());
  Base::setVisible(visible);
}

void
AcAbout::showUpdater()
{
#ifdef AC_ENABLE_UPDATE
  static AcUpdater *updaterDelegate = 0;
  if (!updaterDelegate)
    updaterDelegate = new AcUpdater(this);
  updaterDelegate->open();
#else
  QDesktopServices::openUrl(QUrl(AC_DOWNLOADPAGE));
#endif // AC_ENABLE_UPDATE
}

// EOF
