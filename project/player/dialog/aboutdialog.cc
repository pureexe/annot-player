// aboutdialog.cc
// 11/13/2011

#include "aboutdialog.h"
#include "global.h"
#include "tr.h"
#include "ac/acui.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_WS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( \
    Qt::WindowTitleHint | \
    WINDOW_FLAGS_BASE )
#endif // Q_WS_MAC

// - Constructions -

#ifdef Q_WS_MAC
  #define WINDOW_SIZE   QSize(300, 200)
#else
  #define WINDOW_SIZE   QSize(270, 160)
#endif // Q_WS_MAC

QString
AboutDialog::text()
{
  static const char *ret =
    HTML_CENTER_OPEN()
      HTML_STYLE_OPEN(color:purple)
        HTML_BR()

        G_ORGANIZATION " "
        G_APPLICATION " "
        G_VERSION
        HTML_BR()

        HTML_EM_OPEN()
          G_LICENSE
        HTML_EM_CLOSE()
        HTML_BR()

        HTML_BR()

        HTML_A_OPEN("dummy")
          G_HOMEPAGE      HTML_BR()
          G_UPDATEPAGE    HTML_BR()
          G_EMAIL         HTML_BR()
        HTML_A_CLOSE()

      HTML_STYLE_CLOSE()
    HTML_CENTER_CLOSE()
  ;
  return ret;
}

AboutDialog::AboutDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_ABOUT));

  AcUi::globalInstance()->setWindowStyle(this);

  // Components

  QToolButton *okButton = AcUi::globalInstance()->makeToolButton(
       AcUi::PushHint, TR(T_OK), this, SLOT(hide()));

  textEdit_ = AcUi::globalInstance()->makeTextEdit(
      AcUi::ReadOnlyHint, TR(T_TITLE_ABOUT));
  textEdit_->setHtml(text());

  // Layout

  QVBoxLayout *col = new QVBoxLayout; {
    col->addWidget(textEdit_);
    col->addWidget(okButton, 0, Qt::AlignHCenter); // stretch = 0
  } setLayout(col);

  // l, t, r, b
  int patch = 0;
  if (!AcUi::isAeroAvailable())
    patch = 4;
  col->setContentsMargins(patch, patch, patch, 0);
  setContentsMargins(4, 4, 4, patch);

  resize(WINDOW_SIZE);

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence("CTRL+W"), this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Focus
  okButton->setFocus();
}

// EOF
