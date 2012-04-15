// infoview.cc
// 4/10/2012

#include "infoview.h"
#include "tr.h"
#include "ac/acui.h"
#include "ac/actextview.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

// - Construction -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

InfoView::InfoView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(tr("Information"));
  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence("CTRL+W"), this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *refreshShortcut = new QShortcut(QKeySequence("CTRL+R"), this);
  connect(refreshShortcut, SIGNAL(activated()), SLOT(hide()));
}

void
InfoView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);
  QToolButton *okButton = ui->makeToolButton(
       AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(hide()));
  QToolButton *refreshButton = ui->makeToolButton(
       AcUi::PushHint, TR(T_REFRESH), this, SLOT(refresh()));
  QToolButton *copyButton = ui->makeToolButton(
       AcUi::PushHint, TR(T_COPY), textView_, SLOT(copy()));

  textView_ = new AcTextView;

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *buttons = new QHBoxLayout;
    rows->addWidget(textView_);
    rows->addLayout(buttons);

    buttons->addWidget(copyButton);
    buttons->addWidget(refreshButton);
    buttons->addStretch();
    buttons->addWidget(okButton);

    // l, t, r, b
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 4;
    rows->setContentsMargins(patch, patch, patch, 0);
    setContentsMargins(4, 4, 4, patch);
  } setLayout(rows);

  // Focus
  okButton->setFocus();
}

// - Events -

void
InfoView::setVisible(bool t)
{
  if (t)
    refresh();
  Base::setVisible(t);
}

// EOF
