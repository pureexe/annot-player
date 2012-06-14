// messagehandler.cc
// 10/19/2011

#include "messagehandler.h"
#ifdef WITH_WIN_TEXTHOOK
#  include "win/texthook/texthook.h"
#else
#  error "TextHook module is indispensible"
#endif // WITH_WIN_TEXTHOOK
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/annottag.h"
#include <QtCore>

//#define DEBUG "messagehandler"
#include "module/debug/debug.h"

using namespace AnnotCloud;

//
// Message hashing rule:
//  //- if is subtitle, hash count = 1
//  - if len >= 10, hash count = 1
//  - if len <= 9, look into its previous sentence
//    - if it's previous sentence.len <= 6, look into its previous sentence
//      - if its previous sentence.len <= 3, look into its previous sentence
//  - if repeat, hash count = min(repeat count, 4)
//

namespace { enum { HASH_COUNT = 4}; }

// - Constructions -

MessageHandler::MessageHandler(QObject *parent)
  : Base(parent), active_(false), hookId_(0),
    messageCount_(HASH_COUNT)
{ }

void
MessageHandler::connectTextHook()
{ connect(TextHook::globalInstance(), SIGNAL(textReceived(QString,ulong,ulong)), SLOT(processTextMessage(QString,ulong))); }

void
MessageHandler::disconnectTextHook()
{ disconnect(TextHook::globalInstance(), SIGNAL(textReceived(QString,ulong,ulong)), this, SLOT(processTextMessage(QString,ulong))); }

// - Properties -

void
MessageHandler::setActive(bool active)
{
  if (active_ != active) {
    if (active)
      connectTextHook();
    else {
      disconnectTextHook();
      clearRecentMessages();
    }
    active_ = active;
  }
}

void
MessageHandler::clearRecentMessages()
{
  messages_.clear();
  lastMessageHash_.clear();
}

void
MessageHandler::processTextMessage(const QString &text, ulong hookId)
{
  DOUT("enter: hookId =" << hookId << ", text =" << text);
  if (hookId != hookId_) {
    //lastMessageHash_.clear();
    DOUT("exit: hook mismatch");
    return;
  }

  lastMessageHash_.clear();

  if (text.trimmed().isEmpty()) {
    DOUT("exit: skipping empty text");
    return;
  }

  emit messageReceivedWithText(text);

  messages_.prepend(text);
  if (messages_.size() > messageCount_)
    messages_.removeLast();

  QStringList toHash(text);
  QString t;

  if (messages_.size() >= 2 && (t=messages_[1]) == text) {
    toHash.prepend(t);
    if (messages_.size() >= 3 && (t=messages_[2]) == text) {
      toHash.prepend(t);
      if (messages_.size() >= 4 && (t=messages_[3]) == text)
        toHash.prepend(t);
    }

  } else if (text.length() < 10) {
    if (messages_.size() < 2)
      return;
    toHash.prepend(t = messages_[1]);
    if (t.length() < 7) {
      if (messages_.size() < 3)
        return;
      toHash.prepend(t = messages_[2]);
      if (t.length() < 4) {
        if (messages_.size() < 4)
          return;
        toHash.prepend(messages_[3]);
      }
    }
  }

  int count = toHash.size();
  qint64 h = Annotation::hashTextsPos(toHash);

  lastMessageHash_.hash = h;
  lastMessageHash_.count = count;

  emit messageReceivedWithId(h);
  DOUT("exit: hashCount =" << count << ", h =" << h);
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
    lastMessageHash_.clear();
    lastMessageHash_.count = hashCount;
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
    lastMessageHash_.hash = 0;
    lastMessageHash_.count = hashCount;
    DOUT("processTextMessage:exit: skip zero hash");
  }

  lastMessageHash_.hash = h;
  lastMessageHash_.count = hashCount;
  Q_ASSERT(lastMessageHash_.isValid());

  DOUT("processTextMessage: h =" << h);
  emit messageReceived(h);
  */

