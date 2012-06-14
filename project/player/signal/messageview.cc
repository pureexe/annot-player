// messageview.cc
// 10/16/2011

#include "messageview.h"
#include "textcodecmanager.h"
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

  QString ja = tr("ja"),
          en = tr("en"),
          chs = tr("chs"),
          cht = tr("cht"),
          ko = tr("ko");
  QStringList defaultEncodings = QStringList()
    << QString("SHIFT-JIS [%1]").arg(ja)
    << "UTF-16"
    << "UTF-8"
    << QString("EUC-JP [%1]").arg(ja)
    << QString("ISO-2022-JP [%1]").arg(ja)
    << QString("ISO-8859-1 [%1]").arg(en)
    << QString("GBK [%1]").arg(chs)
    << QString("BIG5 [%1]").arg(cht)
    << QString("EUC-KR [%1]").arg(ko)
    << "UNICODE"
    << TEXT_CODEC_DEFAULT;

  encodingEdit_ = ui->makeComboBox(AcUi::ReadOnlyHint, "", tr("Text Encoding"), tr("Encoding"), defaultEncodings);
  connect(encodingEdit_, SIGNAL(activated(QString)), SLOT(setEncoding(QString)));

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
  connect(hookIndexEdit_, SIGNAL(valueChanged(int)), SLOT(invalidateSelectButton()));

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

    header->addWidget(encodingEdit_);
    header->addWidget(hookIndexEdit_);
    header->addWidget(hookCountLabel_);
    header->addWidget(selectButton_);
    //header->addWidget(autoButton_); // TODO: auto detect is disabled, because hookName is unimplemented
    //header->addStretch();
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
  if (active_) {
    connect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,ulong,ulong)),
            SLOT(processMessage(QByteArray,ulong)));
    connect(TextCodecManager::globalInstance(), SIGNAL(encodingChanged(QString)),
            SLOT(refresh()));
  } else {
    disconnect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,ulong,ulong)),
               this, SLOT(processMessage(QByteArray,ulong)));
    disconnect(TextCodecManager::globalInstance(), SIGNAL(encodingChanged(QString)),
               this, SLOT(refresh()));
  }
#endif // WITH_WIN_TEXTHOOK
}

void
MessageView::refresh()
{
  selectHookIndex(currentIndex());
  refreshEncodingEdit();
}

void
MessageView::refreshEncodingEdit()
{
  int i = encodingEdit_->findText(TextCodecManager::globalInstance()->encoding());
  if (i >= 0 && i < encodingEdit_->count())
    encodingEdit_->setCurrentIndex(i);
}

void
MessageView::setVisible(bool visible)
{
  if (active_ != visible)
    setActive(visible);
  if (visible)
    refreshEncodingEdit();
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
MessageView::addMessages(const QList<QByteArray> &l, ulong hookId)
{
  foreach (const QByteArray &data, l)
    processMessage(data, hookId);
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
  messages_.clear();

  hookIndexEdit_->setValue(0);
  hookIndexEdit_->setMaximum(0);
  hookIndexEdit_->setEnabled(false);
  hooks_.append(0);
  messages_.append(QList<QByteArray>());

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
  //if (index < 0 || index >= messages_.size())
  //  return;

  if (index >= 0 && index < messages_.size())
    setData(messages_[index]);
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
MessageView::processMessage(const QByteArray &data, ulong hookId)
{
  DOUT("enter: hookId =" << hookId << ", data size =" << data.size());

  if (data.isEmpty()) {
    DOUT("exit: skip empty text");
    return;
  }

  int index = hooks_.indexOf(hookId);
  if (index < 0) {
    index = hooks_.size();
    hooks_.append(hookId);
    messages_.append(QList<QByteArray>());
    hookIndexEdit_->setMaximum(index);
    hookIndexEdit_->setEnabled(true);
    invalidateHookCountLabel();

    log(QString("%1 (hid = %2)").arg(tr("new signal discovered")).arg(QString::number(hookId, 16)));

    if (isBetterHook(hookId, currentHookId()))
      setCurrentIndex(index);
  }

  messages_[index].append(data);
  if (index)
    messages_[0].append(data);

  int ci = currentIndex();
  if (ci == index)
    setData(messages_[index]);
  else if (ci == 0)
    setData(messages_[0]);

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
MessageView::setData(const QList<QByteArray> &l)
{
  QString html;
  int i = 0;
  foreach (const QByteArray &b, l) {
    QString s = TextCodecManager::globalInstance()->decode(b);
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

void
MessageView::setEncoding(const QString &name)
{
  QString e = name;
  e.remove(QRegExp(" .*"));
  DOUT("encoding =" << e);
  TextCodecManager::globalInstance()->setEncoding(e);
}

// - Events -

//void MessageView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void MessageView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void MessageView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void MessageView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
