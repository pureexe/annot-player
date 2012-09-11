// messagehandler.cc
// 10/19/2011

#include "messagehandler.h"
#include "textcodecmanager.h"
#ifdef WITH_WIN_TEXTHOOK
# include "win/texthook/texthook.h"
#else
# error "require module texthook"
#endif // WITH_WIN_TEXTHOOK
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/annottag.h"
#include <QtCore>

//#define DEBUG "messagehandler"
#include "module/debug/debug.h"

using namespace AnnotCloud;

enum { GAME_TEXT_MAX = 256 * 2 };

//
// Message hashing rule:
//  //- if is subtitle, hash count = 1
//  - if len >= 20, hash count = 1
//  - if len < 10*2, look into its previous sentence (count >= 2)
//    - if it's previous sentence.len < 7*2, look into its previous sentence (count >= 3)
//      - if its previous sentence.len < 4*2, look into its previous sentence (count = 4)
//  - if repeat, hash count = min(repeat count, 4)
//

namespace { enum { MESSAGE_CAPACITY = 4}; }

// - Constructions -

MessageHandler::MessageHandler(QObject *parent)
  : Base(parent), active_(false), leadingSignature_(0), hash_(0)
{ }

void
MessageHandler::connectTextHook()
{ connect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,qint64,QString)), SLOT(processMessage(QByteArray,qint64))); }

void
MessageHandler::disconnectTextHook()
{ disconnect(TextHook::globalInstance(), SIGNAL(messageReceived(QByteArray,qint64,QString)), this, SLOT(processMessage(QByteArray,qint64))); }

// - Properties -

void
MessageHandler::updateSignatures()
{
  leadingSignature_ = 0;
  if (!supportSignatures_.isEmpty())
    supportSignatures_.clear();

  foreach (const TextThread &t, threads_)
    if (t.isLeadingRole())
      leadingSignature_ = t.signature();
    else
      supportSignatures_.insert(t.signature());
}

void
MessageHandler::setActive(bool active)
{
  if (active_ != active) {
    if (active)
      connectTextHook();
    else {
      disconnectTextHook();
      clearMessages();
    }
    active_ = active;
  }
}

void
MessageHandler::processMessage(const QByteArray &data, qint64 signature)
{
  DOUT("enter: signature =" << signature << ", data size =" << data.size());
  if (data.size() > GAME_TEXT_MAX) {
    if (signature == leadingSignature_) {
      DOUT("main thread text too long, reset");
      clearMessages();
    }
    DOUT("exit: skip long message");
    return;
  }

  if (signature != leadingSignature_) {
    if (!supportSignatures_.isEmpty() && supportSignatures_.contains(signature)) {
      QString text = TextCodecManager::globalInstance()->decode(data);
      emit textChanged(text, TextThread::SupportRole);
    }

    //lastHash_.clear();
    DOUT("exit: hook mismatch");
    return;
  }

  context_.clear();

  if (data.isEmpty()) {
    DOUT("exit: skipping empty data");
    return;
  }

  QString text = TextCodecManager::globalInstance()->decode(data);

  messages_.prepend(data);
  if (messages_.size() > MESSAGE_CAPACITY)
    messages_.pop_back();

  QList<QByteArray> range;
  range.append(data);

  hash_ = Annotation::hash(range);
  qint64 h4 = 0;

  QByteArray t;
  if (messages_.size() >= 2 && (t=messages_[1]) == data) {
    range.prepend(t);
    if (messages_.size() >= 3 && (t=messages_[2]) == data) {
      range.prepend(t);
      if (messages_.size() >= 4 && (t=messages_[3]) == data)
        range.prepend(t);
    }

  } else if (data.length() < 10*2) {
    if (messages_.size() < 2) {
      emit contextChanged(hash_, h4);
      emit textChanged(text, TextThread::LeadingRole);
      DOUT("exit: insufficent messages to hash");
      return;
    }
    range.prepend(t = messages_[1]);
    if (t.length() < 7*2) {
      if (messages_.size() < 3) {
        emit contextChanged(hash_, h4);
        emit textChanged(text, TextThread::LeadingRole);
        DOUT("exit: insufficent messages to hash");
        return;
      }
      range.prepend(t = messages_[2]);
      if (t.length() < 4*2) {
        if (messages_.size() < 4) {
          emit contextChanged(hash_, h4);
          emit textChanged(text, TextThread::LeadingRole);
          DOUT("exit: insufficent messages to hash");
          return;
        }
        range.prepend(messages_[3]);
      }
    }
  }

  h4 = range.size() == data.size() ? hash_ :
       Annotation::hash(range);

  context_.hash = h4;
  context_.count = range.size();

  emit contextChanged(hash_, h4);
  emit textChanged(text, TextThread::LeadingRole);

  DOUT("exit: hashCount =" << context_.count << ", hash =" << hash_ << ", h4 =" << h4);
}

// - Helpers -

//bool
//MessageHandler::isSubtitle(const QString &text)
//{
//  return text.contains(CORE_CMD_SUB " ")
//      || text.contains(CORE_CMD_SUBTITLE " ");
//}

// EOF

  /*
  // Shift hash similar to above messages_
  qint64 h0 = Annotation::hashTextPos(text);
  for (int i = HASH_COUNT - 1; i > 0; i--)
    hashes_[i] = hashes_[i - 1];
  hashes_[0] = h0;
  DOUT("processTextMessage: h0 =" << h0);

  // See the rules on the top
  Q_ASSERT(hashes_.size() >= 4);
  int hashCount = 0;
  switch (text.size()) {
  case 0: Q_ASSERT(0);
  case 1: case 2: case 3: hashCount = 4; break;
  case 4: case 5: case 6: hashCount = 3; break;
  case 7: case 8: case 9: hashCount = 2; break;

  default:
    if (isSubtitle(text))
      hashCount = 1;
    else if (messages_.size() <= 1)
      hashCount = 1;
    else if (messages_.size() >= 2 && text != messages_[1])
      hashCount = 1;
    else if (messages_.size() >= 3 && text != messages_[2])
      hashCount = 2;
    else if (messages_.size() >= 4 && text != messages_[3])
      hashCount = 3;
    else
      hashCount = 4;
  }
  Q_ASSERT(hashCount >=1 && hashCount <= 4);

  if (hashCount > messages_.size()) {
    lastHash_.clear();
    lastHash_.count = hashCount;
    DOUT("processTextMessage:exit: skipping short message: hashCount =" << hashCount << ", messageSize =" << messages_.size());
    return;
  }

  qint64 h = 0;
  switch (hashCount) {
  case 4:
    h = Annotation::rehashTextPos(messages_[2], hashes_[3]);
    h = Annotation::rehashTextPos(messages_[1], h);
    h = Annotation::rehashTextPos(text, h);
  case 3:
    h = Annotation::rehashTextPos(messages_[1], hashes_[2]);
    h = Annotation::rehashTextPos(text, h);
    break;

  case 2: h = Annotation::rehashTextPos(text, hashes_[1]); break;

  case 1: h = h0; break;

  default: Q_ASSERT(0);
  }

  if (!h) {
    qDebug() << "FIXME: Zero hash encountered! text =" << text;
    lastHash_.hash = 0;
    lastHash_.count = hashCount;
    DOUT("processTextMessage:exit: skip zero hash");
  }

  lastHash_.hash = h;
  lastHash_.count = hashCount;
  Q_ASSERT(lastHash_.isValid());

  DOUT("processTextMessage: h =" << h);
  emit messageReceived(h);
  */

