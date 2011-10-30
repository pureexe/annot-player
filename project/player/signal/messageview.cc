// messageview.cc
// 10/16/2011

#include "messageview.h"
#include "textedit.h"
#include "uistyle.h"
#include "tr.h"
#include "global.h"
#include "stylesheet.h"
#include "logger.h"
#include "core/htmltag.h"
#ifdef USE_WIN_QTH
  #include "win/qth/qth.h"
#endif // USE_WIN_QTH
#include "core/cmd.h"
#include "core/gui/toolbutton.h"
#include "core/gui/combobox.h"
#include <QtGui>

#define DEBUG "MessageView"
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

  hookComboBox_ = new Core::Gui::ComboBox; {
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

  QToolButton *resetButton  = new Core::Gui::ToolButton; {
    resetButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    resetButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    resetButton->setText(QString("[ %1 ]").arg(TR(T_RESET)));
    resetButton->setToolTip(tr("Reset changes and texts"));
  }
  connect(resetButton, SIGNAL(clicked()), SLOT(clear()));

  selectButton_ = new Core::Gui::ToolButton; {
    selectButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
    selectButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
    selectButton_->setText(QString("[ %1 ]").arg(TR(T_OK)));
    selectButton_->setToolTip(tr("Use selected signal"));
  }
  connect(selectButton_, SIGNAL(clicked()), SLOT(selectCurrentHook()));

  //cancelButton_ = new Core::Gui::ToolButton; {
  //  cancelButton_->setStyleSheet(SS_TOOLBUTTON_TEXT);
  //  cancelButton_->setToolButtonStyle(Qt::ToolButtonTextOnly);
  //  cancelButton_->setText(QString("[ %1 ]").arg(TR(T_CANCEL)));
  //  cancelButton_->setToolTip(TR(T_CANCEL));
  //}
  //connect(cancelButton_, SIGNAL(clicked()), SLOT(hide()));

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
    header->addWidget(resetButton);

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, 6, 9, 9);
  }
  setLayout(rows);

  // Start up status
  clear();

  invalidateCurrentCharFormat();
  invalidateSelectButton();

  hookComboBox_->setFocus();
}

// - Properties -

bool
MessageView::isActive() const
{ return active_; }

void
MessageView::setActive(bool active)
{
  active_ = active;
#ifdef USE_WIN_QTH
  Q_ASSERT(QTH);
  if (active_)
    connect(QTH, SIGNAL(textReceived(QString,int,qint64)),
            this, SLOT(processHookedText(QString,int)));
  else
    disconnect(QTH, SIGNAL(textReceived(QString,int,qint64)),
               this, SLOT(processHookedText(QString,int)));
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

int
MessageView::currentHookId() const
{
  int hookIndex = currentIndex();
  if (hookIndex >= 0 && hookIndex < hooks_.size())
    return hooks_[hookIndex];
  else
    return 0;
}

// - Actions -

void
MessageView::addMessages(QStringList &messages, int hookId)
{
  foreach (QString text, messages)
    processHookedText(text, hookId);
}

void
MessageView::selectCurrentHook()
{
  int hookId = currentHookId();
  if (hookId) {
    log(tr("process signal selected") + QString(" (hid = %1)").arg(QString::number(hookId, 16)));
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
  hooks_.push_back(0);
  texts_.push_back(QStringList());

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
  int hookId = currentHookId();
  if (hookId)
    selectButton_->setEnabled(true);
  else
    selectButton_->setEnabled(false);
}

void
MessageView::selectHookIndex(int index)
{
  Q_ASSERT(index >= 0 && index < texts_.size());
  //if (index < 0 || index >= texts_.size())
  //  return;

  setTextList(texts_[index]);
}

void
MessageView::processHookedText(const QString &text, int hookId)
{
  DOUT("processHookedText:enter: hookId =" << hookId << ", text =" << text);

  int index = hooks_.indexOf(hookId);
  if (index < 0) {
    index = hooks_.size();
    hooks_.push_back(hookId);
    texts_.push_back(QStringList());
    hookComboBox_->addItem(QString::number(index));
    invalidateHookCountLabel();

    log(tr("new signal discovered") + QString(" (hid = %1)").arg(QString::number(hookId, 16)));
  }

  texts_[index].append(text);
  if (index)
    texts_[0].append(text);

  int ci = currentIndex();
  if (ci == index) {
    setTextList(texts_[index]);
  } else if (ci == 0) {
    setTextList(texts_[0]);
  }

  DOUT("processHookedText:exit");
}

void
MessageView::setTextList(const QStringList &l)
{
  QString html;
  int i = 0;
  foreach (QString s, l) {
    if (i++ % 2)
      html.append(CORE_HTML_STYLE(s, color:purple));
    else
      html.append(CORE_HTML_STYLE(s, color:blue));

    if (i != l.size())
      html.append(CORE_HTML_BR() CORE_HTML_BR());
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
