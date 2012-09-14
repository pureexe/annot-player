// doll.cc
// 8/1/2011

#include "doll.h"
#include "doll_tr.h"
#ifdef WITH_LIB_AIML
# include "lib/aiml/aimlbot.h"
#else
# error "require lib aiml"
#endif // WITH_LIB_AIML

#include <QtCore>

// - Tasks -
namespace { // anonymous, task

  class doll_init_task_ : public QRunnable
  {
    Doll *doll_;
    void run() override {  doll_->setBot(new AimlBot); }

  public:
    doll_init_task_(Doll *doll) : doll_(doll) { Q_ASSERT(doll_); }
  };

  class doll_chat_task_ : public QRunnable
  {
    Doll *doll_;
    QString text_;

    void run() override { doll_->chat(text_, false); }

  public:
    doll_chat_task_(Doll *doll, const QString &text)
      : doll_(doll), text_(text) { Q_ASSERT(doll_); }
  };

} // anonymous namespace

// - Constructions -
Doll::Doll(QObject *parent, bool async)
  : Base(parent), bot_(0)
{
  if (async)
    QThreadPool::globalInstance()->start(
          new doll_init_task_(this));
  else
    setBot(new AimlBot);
}

Doll::~Doll()
{
  QThreadPool::globalInstance()->waitForDone();
  if (bot_)
    delete bot_;
}

bool
Doll::isReady() const
{ return bot_; }

AimlBot*
Doll::bot() const
{ return bot_; }

void
Doll::setBot(AimlBot *bot)
{
  bot_ = bot;
  if (bot_)
    emit ready();
}

// - Chat -

void
Doll::chat(const QString &text, bool async)
{
  if (async) {
    //QThreadPool::globalInstance()->waitForDone(); // force asynchronize
    QThreadPool::globalInstance()->start(
          new doll_chat_task_(this, text));

  } else {
    QString reply;
    if (isReady())
      reply = bot_->chat(text);
    else
      reply = TR_DOLL_RESPONSE_BUSY;

    emit said(reply);
  }
}

// EOF
