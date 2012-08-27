// messageview.cc
// 10/16/2011

#include "messageview.h"
#include "radiobuttongrid.h"
#include "textcodecmanager.h"
#include "tr.h"
#include "global.h"
#ifdef WITH_WIN_TEXTHOOK
# include "win/texthook/texthook.h"
#endif // WITH_WIN_TEXTHOOK
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/spinbox.h"
#include <QtGui>

//#define DEBUG "messageview"
#include "module/debug/debug.h"

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

#define HOOKCOMBOBOX_MINWIDTH       40
#define HOOKCOMBOBOX_MAXWIDTH       60

// - Construction -

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


  enum { MaxChannelCount = 50, GridColumn = 4 };
  channelGrid_ = new RadioButtonGrid(MaxChannelCount, GridColumn, this);
  connect(channelGrid_, SIGNAL(valueChanged(int)), SLOT(setCurrentText(int)));
  connect(channelGrid_, SIGNAL(valueChanged(int)), SLOT(updateButtons()));

  textEdit_ = ui->makeTextEdit(AcUi::ReadOnlyHint, tr("Process message")); {
    //QTextCharFormat fmt;
    //fmt.setBackground(Qt::red);
    //QTextCursor tc = textEdit_->textCursor();
    //tc.mergeCharFormat(fmt);
    //textEdit_->setTextCursor(tc);
  }
  //connect(textEdit_, SIGNAL(cursorPositionChanged()), SLOT(invalidateCurrentCharFormat()));

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

  //hookIndexEdit_ = new QtExt::SpinBox; {
  //  hookIndexEdit_->setToolTip(tr("Signal channel"));
  //  //hookIndexEdit_->setMinimumWidth(HOOKCOMBOBOX_MINWIDTH);
  //  //hookIndexEdit_->setMaximumWidth(HOOKCOMBOBOX_MAXWIDTH);
  //  //if (hookIndexEdit_->isEditable())
  //  //  hookIndexEdit_->lineEdit()->setAlignment(Qt::AlignRight);
  //  hookIndexEdit_->setMaximum(0);
  //  hookIndexEdit_->setMinimum(0);
  //  hookIndexEdit_->setEnabled(false);
  //}

  //autoButton_ = ui->makeToolButton(
  //      AcUi::CheckHint, TR(T_AUTO), tr("Auto-detect signal"), this, SLOT(invalidateCurrentHook()));
  //autoButton_->setChecked(false);

  selectButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint | AcUi::InvertHint, TR(T_OK), tr("Listen to selected channel"), this, SLOT(select()));

  resetButton_ = ui->makeToolButton(
        AcUi::PushHint, TR(T_RESET), tr("Reset changes and texts"), this, SLOT(clear()));

  //hookCountLabel_ = ui->makeLabel(0, "/0", tr("Current signal"), hookIndexEdit_);

  // Set layout

  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(channelGrid_->layout());
    rows->addWidget(textEdit_);

    header->addWidget(encodingEdit_);
    //header->addWidget(hookIndexEdit_);
    //header->addWidget(hookCountLabel_);
    //header->addWidget(autoButton_); // TODO: auto detect is disabled, because hookName is unimplemented
    //header->addStretch();
    header->addWidget(resetButton_);
    header->addWidget(selectButton_);

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, 6, 9, 9);
  } setLayout(rows);

  // Start up status
  clear();

  //invalidateCurrentCharFormat();
  updateButtons();

  encodingEdit_->setFocus();
  //hookIndexEdit_->setFocus();
}

// - Properties -

void
MessageView::setActive(bool active)
{
  active_ = active;
#ifdef WITH_WIN_TEXTHOOK
  if (active_) {
    connect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,ulong,QString)),
            SLOT(processMessage(QByteArray,ulong,QString)));
    connect(TextCodecManager::globalInstance(), SIGNAL(encodingChanged(QString)),
            SLOT(refresh()));
  } else {
    disconnect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,ulong,QString)),
               this, SLOT(processMessage(QByteArray,ulong,QString)));
    disconnect(TextCodecManager::globalInstance(), SIGNAL(encodingChanged(QString)),
               this, SLOT(refresh()));
  }
#endif // WITH_WIN_TEXTHOOK
}

void
MessageView::refresh()
{
  setCurrentText(currentIndex());
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

bool
MessageView::isEmpty() const
{ return channelGrid_->isEmpty(); }

int
MessageView::currentIndex() const
{ return channelGrid_->value(); }

void
MessageView::setCurrentIndex(int index)
{
  channelGrid_->setValue(index);
  updateButtons();
}

ulong
MessageView::currentAnchor() const
{
  int index = currentIndex();
  return index < 0 || index >= anchors_.size() ? 0
       : anchors_[index];
}

QString
MessageView::currentFunction() const
{
  int index = currentIndex();
  return index < 0 || index >= functions_.size() ? 0
       : functions_[index];
}

// - Actions -

void
MessageView::addMessages(const QList<QByteArray> &l, ulong anchor, const QString &function)
{
  foreach (const QByteArray &data, l)
    processMessage(data, anchor, function);
}

void
MessageView::select()
{
  ulong anchor = currentAnchor();
  if (anchor) {
    QString func = currentFunction();
    emit message(tr("channel selected") + ": " + func);
    emit channelSelected(anchor, func);
  }
}

void
MessageView::clear()
{
  channelGrid_->clear();
  anchors_.clear();
  functions_.clear();
  messages_.clear();

  //hookIndexEdit_->setValue(0);
  //hookIndexEdit_->setMaximum(0);
  //hookIndexEdit_->setEnabled(false);
  //anchors_.append(0);
  //messages_.append(QList<QByteArray>());

  textEdit_->clear();

  //invalidateHookCountLabel();
  updateButtons();

  emit message(tr("messages cleared"));
}

//void
//MessageView::invalidateHookCountLabel()
//{
//  int count = anchors_.size() - 1;
//  hookCountLabel_->setText(QString("/%1 ").arg(QString::number(count)));
//}

void
MessageView::updateButtons()
{
  bool t = currentIndex() > 0;
  selectButton_->setEnabled(t);
  resetButton_->setEnabled(t);
}

void
MessageView::setCurrentText(int index)
{
  //if (index < 0 || index >= messages_.size())
  //  return;

  if (index >= 0 && index < messages_.size())
    setData(messages_[index]);
}

//bool
//MessageView::isBetterHook(ulong goodHookId, ulong badHookId)
//{
//  QString badHookName = TextHook::globalInstance()->hookNameById(badHookId);
//  if (badHookName.isEmpty())
//    return true;
//  QString goodHookName = TextHook::globalInstance()->hookNameById(goodHookId);
//  if (goodHookName.isEmpty())
//    return false;
//
//  return TextHook::globalInstance()->isStandardHookName(badHookName) ||
//         !TextHook::globalInstance()->isStandardHookName(goodHookName);
//}

void
MessageView::processMessage(const QByteArray &data, ulong anchor, const QString &function)
{
  DOUT("enter: anchor =" << anchor << ", data size =" << data.size());

  if (data.isEmpty() || !anchor) {
    DOUT("exit: skip empty message");
    return;
  }

  if (isEmpty()) {
    channelGrid_->addItem(tr("All"));
    anchors_.append(0);
    functions_.append(QString());
    messages_.append(QList<QByteArray>());
  }

  int index = anchors_.indexOf(anchor);
  if (index < 0) {
    index = anchors_.size();
    anchors_.append(anchor);
    functions_.append(function);
    messages_.append(QList<QByteArray>());
    channelGrid_->addItem(function);
    //invalidateHookCountLabel();

    emit message(tr("new channel discovered") + ": " + function);

    //if (isBetterHook(id, currentHookId()))
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

  //invalidateCurrentHook();

  DOUT("exit");
}

//void
//MessageView::invalidateCurrentHook()
//{
//  if (processName_.isEmpty())
//    return;
//
//  ulong hid = currentHookId();
//  QString hookName = TextHook::globalInstance()->hookNameById(hid);
//  if (!hookName.isEmpty() && TextHook::globalInstance()->isKnownHookForProcess(hookName, processName_))
//    selectCurrentHook();
//}

void
MessageView::setData(const QList<QByteArray> &l)
{
  QString html;
  int i = 0;
  foreach (const QByteArray &b, l) {
    QString s = TextCodecManager::globalInstance()->decode(b);
    if (i++ % 2)
      html.append(HTML_SS(+s+, color:purple));
    else
      html.append(HTML_SS(+s+, color:blue));

    if (i != l.size())
      html.append(HTML_BR() HTML_BR());
  }

  textEdit_->setHtml(html);

  //QTextCursor tc = textEdit_->textCursor();
  //tc.movePosition(QTextCursor::End);
  //textEdit_->setTextCursor(tc);
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
