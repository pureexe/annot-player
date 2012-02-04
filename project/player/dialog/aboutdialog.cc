// aboutdialog.cc
// 11/13/2011

#include "aboutdialog.h"
#include "uistyle.h"
#include "defines.h"
#include "tr.h"
#include "stylesheet.h"
#include "util/textedit.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

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
  : Base(parent)
{
#ifdef Q_WS_MAC
  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#endif // Q_WS_MAC
  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowTitle(TR(T_TITLE_ABOUT));

  // Components

  QToolButton *okButton = new QtExt::ToolButton; {
    okButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    okButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    okButton->setText(QString("[ %1 ]").arg(TR(T_OK)));
    okButton->setToolTip(TR(T_OK));
  }
  connect(okButton, SIGNAL(clicked()), SLOT(ok()));

  textEdit_ = new TextEdit; {
    textEdit_->setReadOnly(true);
    textEdit_->setHtml(text());
  }

  // Layout

  QVBoxLayout *col = new QVBoxLayout; {
    col->addWidget(textEdit_);
    col->addWidget(okButton, 0, Qt::AlignHCenter); // stretch = 0
  } setLayout(col);

  // l, t, r, b
  int patch = 0;
  if (!UiStyle::isAeroAvailable())
    patch = 4;
  col->setContentsMargins(patch, patch, patch, 0);
  setContentsMargins(4, 4, 4, patch);

  resize(WINDOW_SIZE);

  // Focus
  okButton->setFocus();
}

// - Slots -

void
AboutDialog::ok()
{ hide(); }

// EOF
