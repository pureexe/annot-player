// messageview.cc
// 10/16/2011

#include "messageview.h"
#include "textedit.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "logger.h"
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#include "module/annotcloud/cmd.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/combobox.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);

  textEdit_ = new TextEdit; {
    textEdit_->setStyleSheet(SS_TEXTEDIT);
    textEdit_->setToolTip(tr("Process messages"));

    //QTextCharFormat fmt;
    //fmt.setBackground(QColor("red"));
    //QTextCursor tc = textEdit_->textCursor();
    //tc.mergeCharFormat(fmt);
    //textEdit_->setTextCursor(tc);
  }
  connect(textEdit_, SIGNAL(cursorPositionChanged()), SLOT(invalidateCurrentCharFormat()));

  hookComboBox_ = new QtExt::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(hookComboBox_);
    hookComboBox_->setEditable(false);
    hookComboBox_->setMinimumWidth(HOOKCOMBOBOX_MINWIDTH);
    hookComboBox_->setMaximumWidth(HOOKCOMBOBOX_MAXWIDTH);
    hookComboBox_->setToolTip(tr("Signal channel"));
    //if (hookComboBox_->isEditable())
    //  hookComboBox_->lineEdit()->setAlignment(Qt::AlignRight);
  }
  connect(hookComboBox_, SIGNAL(activated(int)), SLOT(selectHookIndex(int)));
  connect(hookComboBox_, SIGNAL(currentIndexChanged(int)), SLOT(invalidateSelectButton()));

  autoButton_  = new QtExt::ToolButton; {
    autoButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
    autoButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
    autoButton_->setText(QString("| %1 |").arg(TR(T_AUTO)));
    autoButton_->setToolTip(tr("Auto-detect signal"));
    autoButton_->setCheckable(true);
    autoButton_->setChecked(true);
  }
  connect(autoButton_, SIGNAL(clicked(bool)), SLOT(invalidateCurrentHook()));

  QToolButton *resetButton  = new QtExt::ToolButton; {
    resetButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    resetButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    resetButton->setText(QString("[ %1 ]").arg(TR(T_RESET)));
    resetButton->setToolTip(tr("Reset changes and texts"));
  }
  connect(resetButton, SIGNAL(clicked()), SLOT(clear()));

  selectButton_ = new QtExt::ToolButton; {
    selectButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
    selectButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
    selectButton_->setText(QString("[ %1 ]").arg(TR(T_OK)));
    selectButton_->setToolTip(tr("Use selected signal"));
  }
  connect(selectButton_, SIGNAL(clicked()), SLOT(selectCurrentHook()));

  hookCountLabel_ = new QLabel; {
    hookCountLabel_->setStyleSheet(SS_LABEL);
    hookCountLabel_->setBuddy(hookComboBox_);
    hookCountLabel_->setToolTip(tr("Current signal"));
  }

  // Set layout

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(textEdit_);

    header->addWidget(hookComboBox_);
    header->addWidget(hookCountLabel_);
    header->addWidget(selectButton_);
    header->addStretch();
    header->addWidget(autoButton_);
    header->addWidget(resetButton);

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, 6, 9, 9);
  } setLayout(rows);

  // Start up status
  clear();

  invalidateCurrentCharFormat();
  invalidateSelectButton();

  hookComboBox_->setFocus();
}

// - Properties -

void
MessageView::setActive(bool active)
{
  active_ = active;
#ifdef USE_WIN_QTH
  Q_ASSERT(QTH);
  if (active_)
    connect(QTH, SIGNAL(textReceived(QString,ulong,ulong,QString)),
            this, SLOT(processHookedText(QString,ulong)));
  else
    disconnect(QTH, SIGNAL(textReceived(QString,ulong,ulong,QString)),
               this, SLOT(processHookedText(QString,ulong)));
#endif USE_WIN_QTH
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
{ return hookComboBox_->currentIndex(); }

void
MessageView::setCurrentIndex(int index)
{
  hookComboBox_->setCurrentIndex(index);
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
  foreach (QString text, messages)
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
  hookComboBox_->clear();
  hooks_.clear();
  texts_.clear();

  hookComboBox_->addItem("0");
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
  hookCountLabel_->setText(QString(" %1 ").arg(QString::number(count)));
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
  Q_ASSERT(index >= 0 && index < texts_.size());
  //if (index < 0 || index >= texts_.size())
  //  return;

  setTextList(texts_[index]);
}

bool
MessageView::isBetterHook(ulong goodHookId, ulong badHookId)
{
  QString badHookName = QTH->hookNameById(badHookId);
  if (badHookName.isEmpty())
    return true;
  QString goodHookName = QTH->hookNameById(goodHookId);
  if (goodHookName.isEmpty())
    return false;

  if (QTH->isStandardHookName(badHookName))
    return true;
  return !QTH->isStandardHookName(goodHookName);
}

void
MessageView::processHookedText(const QString &text, ulong hookId)
{
  DOUT("enter: hookId =" << hookId << ", text =" << text);

  int index = hooks_.indexOf(hookId);
  if (index < 0) {
    index = hooks_.size();
    hooks_.append(hookId);
    texts_.append(QStringList());
    hookComboBox_->addItem(QString::number(index));
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
  QString hookName = QTH->hookNameById(hid);
  if (!hookName.isEmpty() && QTH->isKnownHookForProcess(hookName, processName_))
    selectCurrentHook();
}

void
MessageView::setTextList(const QStringList &l)
{
  QString html;
  int i = 0;
  foreach (QString s, l) {
    if (i++ % 2)
      html.append(HTML_STYLE(s, color:purple));
    else
      html.append(HTML_STYLE(s, color:blue));

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
  fmt.setForeground(QColor("red"));
  textEdit_->mergeCurrentCharFormat(fmt);
}

// - Events -

void MessageView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void MessageView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void MessageView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void MessageView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
