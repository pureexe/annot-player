// hookdialog.cc
// 9/2/2012

#include "hookdialog.h"
#include "tr.h"
#include "game.h"
#include "src/common/acui.h"
#include "src/common/accomboedit.h"
#include "htmlutil/sstags.h"
#include "qtx/qxlayoutwidget.h"
#include "win/texthook/texthook.h"
#include <QtGui>

#define DEBUG "hookdialog"
#include "qtx/qxdebug.h"

// - Constructions -

// Koichoko, fack MSVC orz
#define SAMPLE_GAME "\xe6\x81\x8b\xe3\x81\xa8\xe9\x81\xb8\xe6\x8c\x99\xe3\x81\xa8\xe3\x83\x81\xe3\x83\xa7\xe3\x82\xb3\xe3\x83\xac\xe3\x83\xbc\xe3\x83\x88"
#define SAMPLE_HOOK "/HSN-4@B14B4!F20AE0D2"


#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

HookDialog::HookDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(tr("Import ITH Hook Code"));
  createLayout();
}

void
HookDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  edit_ = ui->makeComboBox(AcUi::EditHint, "", tr("ITH Hook Code"), "e.g. " SAMPLE_HOOK);
  editStyleSheet_ = edit_->styleSheet();
  connect(edit_, SIGNAL(editTextChanged(QString)), SLOT(verifyEditText()));
  connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(open()));
  auto edit = qobject_cast<AcComboEdit *>(edit_);
  Q_ASSERT(edit);
  edit->setContextMenuFlags(edit->contextMenuFlags() | AcComboEdit::PasteAndGoAction);

  //saveButton_ = ui->makeToolButton(
  //      AcUi::CheckHint, TR(T_SAVE), tr("Save the association online"));
  //saveButton_->setChecked(true);

  QLabel *hookLabel = ui->makeLabel(AcUi::HighlightHint, SAMPLE_HOOK),
         *hookBuddy = ui->makeLabel(AcUi::BuddyHint, TR(T_EXAMPLE) + " (" SAMPLE_GAME ")", hookLabel);

  openButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_ADD), this, SLOT(open()));
  QToolButton *pasteButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_PASTE), this, SLOT(paste()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), edit_->lineEdit(), SLOT(clear()));

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;

    rows->addLayout(header);
    rows->addWidget(edit_);
    rows->addLayout(footer);

    header->addWidget(hookBuddy);
    header->addWidget(hookLabel);

    footer->addWidget(clearButton);
    footer->addWidget(pasteButton);
    footer->addStretch();
    //footer->addWidget(saveButton_);
    footer->addWidget(openButton_);

    rows->setContentsMargins(4, 4, 4, 4);
    setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new QxLayoutWidget(rows, this));

  setStyleSheet(styleSheet() +
    SS_BEGIN(QToolButton)
      SS_NO_BORDER
    SS_END
  );
}

// - Slots -

void
HookDialog::open()
{
  updateEnabled();
  if (!isEnabled())
    return;

  QString hook = text().trimmed();
  if (!Game::isValidHook(hook))
    return;

  ulong pid = TextHook::globalInstance()->currentProccess();
  if (!pid) {
    fadeOut();
    emit warning(tr("not synchronized with any game"));
    return;
  }
  if (TextHook::globalInstance()->containsHook(pid, hook)) {
    fadeOut();
    emit message(tr("hook code already added") + ": " + hook);
    return;
  }
  if (!TextHook::globalInstance()->addHook(pid, hook)) {
    warn("invalid hook code for the current game");
    return;
  }

  fadeOut();
  if (edit_->count() &&
      edit_->itemText(edit_->count() - 1) != hook)
    edit_->addItem(hook);

  emit message(tr("add text hook") + ": " + hook);
  emit hookEntered(hook);
}

void
HookDialog::paste()
{
  QClipboard *c = QApplication::clipboard();
  if (c){
    QString t = c->text().trimmed();
    if (!t.isEmpty())
      setText(t);
  }
}

QString
HookDialog::text() const
{ return edit_->currentText(); }

bool
HookDialog::isEmpty() const
{ return text().trimmed().isEmpty(); }

void
HookDialog::setText(const QString &text)
{ edit_->setEditText(text); }

void
HookDialog::updateEnabled()
{ setEnabled(TextHook::globalInstance()->currentHook().isEmpty()); }

void
HookDialog::refresh()
{
  updateEnabled();
  if (isEnabled())
    paste();
  else
    setText(TextHook::globalInstance()->currentHook());
  verifyEditText();
  if (!isEnabled()) {
    showMessage(tr("ITH hook is already imported"));
    DOUT("current hook =" << text());
  }
}

// - Events -

void
HookDialog::setVisible(bool visible)
{
  if (visible) {
    refresh();
    edit_->setFocus();
  }
  Base::setVisible(visible);
}

void
HookDialog::verifyEditText()
{
  QString t = edit_->currentText().trimmed();
  bool valid = Game::isValidHook(t);

  openButton_->setEnabled(valid);
  edit_->setStyleSheet(editStyleSheet_ + (valid ?
    SS_BEGIN(QComboBox)
      SS_COLOR(black)
    SS_END
    :
    SS_BEGIN(QComboBox)
      SS_COLOR(red)
    SS_END
  ));

  if (valid)
    showMessage(tr("Valid") + ": " + t);
  else if (!t.isEmpty())
    warn(tr("Invalid") + ": " + t);
}

// EOF
