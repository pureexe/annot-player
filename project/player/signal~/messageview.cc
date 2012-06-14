// messageview.cc
// 10/16/2011

#include "messageview.h"
#include "tr.h"
#include "global.h"
#include "logger.h"
#ifdef WITH_WIN_TEXTHOOK
#  include "win/texthook/texthook.h"
#endif // WITH_WIN_TEXTHOOK
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/spinbox.h"
#include <QtGui>

//#define DEBUG "messageview"
#include "module/debug/debug.h"

using namespace Logger;

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#define HOOKCOMBOBOX_MINWIDTH       40
#define HOOKCOMBOBOX_MAXWIDTH       60

// - Constructions -

MessageView::MessageView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), active_(false)
{
  setWindowTitle(tr("Message view"));

  createLayout();
}

void
MessageView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  textEdit_ = ui->makeTextEdit(0, tr("Process message")); {
    //QTextCharFormat fmt;
    //fmt.setBackground(Qt::red);
    //QTextCursor tc = textEdit_->textCursor();
    //tc.mergeCharFormat(fmt);
    //textEdit_->setTextCursor(tc);
  }
  connect(textEdit_, SIGNAL(cursorPositionChanged()), SLOT(invalidateCurrentCharFormat()));

  hookIndexEdit_ = new QtExt::SpinBox; {
    hookIndexEdit_->setToolTip(tr("Signal channel"));
    //hookIndexEdit_->setMinimumWidth(HOOKCOMBOBOX_MINWIDTH);
    //hookIndexEdit_->setMaximumWidth(HOOKCOMBOBOX_MAXWIDTH);
    //if (hookIndexEdit_->isEditable())
    //  hookIndexEdit_->lineEdit()->setAlignment(Qt::AlignRight);
    hookIndexEdit_->setMaximum(0);
    hookIndexEdit_->setMinimum(0);
    hookIndexEdit_->setEnabled(false);
  }
  connect(hookIndexEdit_, SIGNAL(valueChanged(int)), SLOT(selectHookIndex(int)));

  autoButton_ = ui->makeToolButton(
        AcUi::CheckHint, TR(T_AUTO), tr("Auto-detect signal"), this, SLOT(invalidateCurrentHook()));
  autoButton_->setChecked(false);

  selectButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), tr("Use selected signal"), this, SLOT(selectCurrentHook()));

  QToolButton *resetButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_RESET), tr("Reset changes and texts"), this, SLOT(clear()));

  hookCountLabel_ = ui->makeLabel(0, "/0", tr("Current signal"), hookIndexEdit_);

  // Set layout

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(textEdit_);

    header->addWidget(hookIndexEdit_);
    header->addWidget(hookCountLabel_);
    header->addWidget(selectButton_);
    header->addStretch();
    //header->addWidget(autoButton_); // TODO: auto detect is disabled, because hookName is unimplemented
    header->addWidget(resetButton);

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, 6, 9, 9);
  } setLayout(rows);

  // Start up status
  clear();

  invalidateCurrentCharFormat();
  invalidateSelectButton();

  hookIndexEdit_->setFocus();
}

// - Properties -

void
MessageView::setActive(bool active)
{
  active_ = active;
#ifdef WITH_WIN_TEXTHOOK
  if (active_)
    connect(TextHook::globalInstance(), SIGNAL(textReceived(QString,ulong,ulong)),
            this, SLOT(processHookedText(QString,ulong)));
  else
    disconnect(TextHook::globalInstance(), SIGNAL(textReceived(QString,ulong,ulong)),
               this, SLOT(processHookedText(QString,ulong)));
#endif // WITH_WIN_TEXTHOOK
}

void
MessageView::setVisible(bool visible)
{
  if (active_ != visible)
    setActive(visible);
  Base::setVisible(visible);
}

int
MessageView::currentIndex() const
{ return hookIndexEdit_->value(); }

void
MessageView::setCurrentIndex(int index)
{
  hookIndexEdit_->setValue(index);
  invalidateSelectButton();
}

ulong
MessageView::currentHookId() const
{
  int hookIndex = currentIndex();
  return hookIndex < 0 || hookIndex >= hooks_.size() ? 0
       : hooks_[hookIndex];
}

// - Actions -

void
MessageView::addMessages(QStringList &messages, ulong hookId)
{
  foreach (const QString &text, messages)
    processHookedText(text, hookId);
}

void
MessageView::selectCurrentHook()
{
  ulong hookId = currentHookId();
  if (hookId) {
    log(QString("%1 (hid = %2)").arg(tr("process signal selected")).arg(QString::number(hookId, 16)));
    emit hookSelected(hookId);
  }
}

void
MessageView::clear()
{
  hookIndexEdit_->clear();
  hooks_.clear();
  texts_.clear();

  hookIndexEdit_->setValue(0);
  hookIndexEdit_->setMaximum(0);
  hookIndexEdit_->setEnabled(false);
  hooks_.append(0);
  texts_.append(QStringList());

  textEdit_->clear();

  invalidateHookCountLabel();
  invalidateSelectButton();
}

void
MessageView::invalidateHookCountLabel()
{
  int count = hooks_.size() - 1;
  hookCountLabel_->setText(QString("/%1 ").arg(QString::number(count)));
}

void
MessageView::invalidateSelectButton()
{
  ulong hookId = currentHookId();
  selectButton_->setEnabled(hookId);
}

void
MessageView::selectHookIndex(int index)
{
  //if (index < 0 || index >= texts_.size())
  //  return;

  if (index >= 0 && index < texts_.size())
    setTextList(texts_[index]);
}

bool
MessageView::isBetterHook(ulong goodHookId, ulong badHookId)
{
  QString badHookName = TextHook::globalInstance()->hookNameById(badHookId);
  if (badHookName.isEmpty())
    return true;
  QString goodHookName = TextHook::globalInstance()->hookNameById(goodHookId);
  if (goodHookName.isEmpty())
    return false;

  return TextHook::globalInstance()->isStandardHookName(badHookName) ||
         !TextHook::globalInstance()->isStandardHookName(goodHookName);
}

void
MessageView::processHookedText(const QString &text, ulong hookId)
{
  DOUT("enter: hookId =" << hookId << ", text =" << text);

  if (text.trimmed().isEmpty()) {
    DOUT("exit: skip empty text");
    return;
  }

  int index = hooks_.indexOf(hookId);
  if (index < 0) {
    index = hooks_.size();
    hooks_.append(hookId);
    texts_.append(QStringList());
    hookIndexEdit_->setMaximum(index);
    hookIndexEdit_->setEnabled(true);
    invalidateHookCountLabel();

    log(QString("%1 (hid = %2)").arg(tr("new signal discovered")).arg(QString::number(hookId, 16)));

    if (isBetterHook(hookId, currentHookId()))
      setCurrentIndex(index);
  }

  texts_[index].append(text);
  if (index)
    texts_[0].append(text);

  int ci = currentIndex();
  if (ci == index)
    setTextList(texts_[index]);
  else if (ci == 0)
    setTextList(texts_[0]);

  invalidateCurrentHook();

  DOUT("exit");
}

void
MessageView::invalidateCurrentHook()
{
  if (!autoButton_->isChecked() || processName_.isEmpty())
    return;

  ulong hid = currentHookId();
  QString hookName = TextHook::globalInstance()->hookNameById(hid);
  if (!hookName.isEmpty() && TextHook::globalInstance()->isKnownHookForProcess(hookName, processName_))
    selectCurrentHook();
}

void
MessageView::setTextList(const QStringList &l)
{
  QString html;
  int i = 0;
  foreach (const QString &s, l) {
    if (i++ % 2)
      html.append(HTML_STYLE(+s+, color:purple));
    else
      html.append(HTML_STYLE(+s+, color:blue));

    if (i != l.size())
      html.append(HTML_BR() HTML_BR());
  }

  textEdit_->setHtml(html);

  QTextCursor tc = textEdit_->textCursor();
  tc.movePosition(QTextCursor::End);
  textEdit_->setTextCursor(tc);
}

void
MessageView::invalidateCurrentCharFormat()
{
  QTextCharFormat fmt;
  fmt.setForeground(Qt::red);
  textEdit_->mergeCurrentCharFormat(fmt);
}

// - Events -

//void MessageView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void MessageView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void MessageView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void MessageView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
