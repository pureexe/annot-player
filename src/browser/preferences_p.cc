// preferences_p.cc
// 5/25/2012

#include "preferences_p.h"
#include "src/common/acui.h"
#include "htmlutil/htmltags.h"
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

// - Construction -

#ifdef _MSC_VER
# define S_NoMatch  "\xce\xa6"
# define S_Up       "\xe2\x86\x91"
# define S_Down     "\xe2\x86\x93"
# define S_Left     "\xe2\x86\x90"
# define S_Right    "\xe2\x86\x92"
#else
# define S_NoMatch  "Φ"
# define S_Up       "↑"
# define S_Down     "↓"
# define S_Left     "←"
# define S_Right    "→"
#endif // _MSC_VER

enum { MIN_HEIGHT = 250 };

GesturePreferencesTab::GesturePreferencesTab(QWidget *parent)
  : Base(parent)
{
  setWindowTitle(tr("Gesture"));
  createLayout();

  textEdit_->setText(
HTML_CENTER_OPEN()
HTML_TABLE_OPEN()
  HTML_TR(HTML_TH(+tr("Function")+) HTML_TH(+tr("Gesture")+))

  HTML_TR(HTML_TD(+tr("Open Link in Tab")+)    HTML_TD(S_Down))
  HTML_TR(HTML_TD(+tr("Previous in History")+) HTML_TD(S_Left))
  HTML_TR(HTML_TD(+tr("Next in History")+)     HTML_TD(S_Right))
  HTML_TR(HTML_TD(+tr("Previous Tab")+)        HTML_TD(S_Up " " S_Left))
  HTML_TR(HTML_TD(+tr("Next Tab")+)            HTML_TD(S_Up " " S_Right))
  HTML_TR(HTML_TD(+tr("Close Tab")+)           HTML_TD(S_Down " " S_Right))
  HTML_TR(HTML_TD(+tr("New Tab")+)             HTML_TD(S_Right " " S_Left))
  HTML_TR(HTML_TD(+tr("Undo Close Tab")+)      HTML_TD(S_Left " " S_Right))
  HTML_TR(HTML_TD(+tr("Refresh")+)             HTML_TD(S_Up " " S_Down))
  HTML_TR(HTML_TD(+tr("Full Screen")+)         HTML_TD(S_Right " " S_Up))
  HTML_TR(HTML_TD(+tr("Minimize")+)            HTML_TD(S_Right " " S_Down))
  HTML_TR(HTML_TD(+tr("Scroll Top")+)          HTML_TD(S_Left " " S_Up))
  HTML_TR(HTML_TD(+tr("Scroll Bottom")+)       HTML_TD(S_Left " " S_Down))
HTML_TABLE_CLOSE()
HTML_CENTER_CLOSE()
  );

  textEdit_->setMinimumHeight(MIN_HEIGHT);
}

void
GesturePreferencesTab::createLayout()
{
  textEdit_ = AcUi::globalInstance()->makeTextEdit(AcUi::ReadOnlyHint, tr("Read-only"));
  QLayout *layout = new QVBoxLayout;
  layout->addWidget(textEdit_);
  setLayout(layout);
}

// EOF
