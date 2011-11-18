// aboutdialog.cc
// 11/13/2011

#include "aboutdialog.h"
#include "uistyle.h"
#include "global.h"
#include "tr.h"
#include "stylesheet.h"
#include "util/textedit.h"
#include "core/htmltag.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

// - Constructions -

#define WINDOW_SIZE   QSize(270, 160)

QString
AboutDialog::text()
{
  static const char *ret =
    CORE_HTML_CENTER_OPEN()
      CORE_HTML_STYLE_OPEN(color:purple)
        CORE_HTML_BR()

        G_ORGANIZATION " "
        G_APPLICATION " "
        G_VERSION
        CORE_HTML_BR()

        CORE_HTML_EM_OPEN()
          G_LICENSE
        CORE_HTML_EM_CLOSE()
        CORE_HTML_BR()

        CORE_HTML_BR()

        CORE_HTML_A_OPEN("dummy")
          G_HOMEPAGE      CORE_HTML_BR()
          G_UPDATEPAGE    CORE_HTML_BR()
          G_EMAIL         CORE_HTML_BR()
        CORE_HTML_A_CLOSE()

      CORE_HTML_STYLE_CLOSE()
    CORE_HTML_CENTER_CLOSE()
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

  QToolButton *okButton = new Core::Gui::ToolButton; {
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
  }
  setLayout(col);

  // l, t, r, b
  col->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(4, 4, 4, 0);

  resize(WINDOW_SIZE);

  // Focus
  okButton->setFocus();
}

// - Slots -

void
AboutDialog::ok()
{ hide(); }

// EOF
