#ifndef AIMLBOT_H
#define AIMLBOT_H

// aimlbot.h
// 7/31/2011

#include <QString>

class AimlParser;
class AimlBot
{
  AimlParser *parser_; // Ready to use until \fn ready().

public:
  ///  If \p async, perform initialization from another thread.
  explicit AimlBot(bool showProgressDialog = false);
  ~AimlBot();

  AimlParser *parser() const;
  void setParser(AimlParser *parser);
public:
  QString chat(const QString &text);
};

#endif // AIMLBOT_H
