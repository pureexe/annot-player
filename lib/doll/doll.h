#ifndef DOLL_H
#define DOLL_H

// doll.h
// 8/1/2011

#include <QtCore/QObject>

class AimlBot;
class Doll : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Doll)
  typedef Doll Self;
  typedef QObject Base;

  AimlBot *bot_; // Ready to use until \fn ready().

public:
  ///  If \p async, perform initialization from another thread.
  explicit Doll(QObject *parent = nullptr, bool async = true);
  ~Doll();

signals:
  void ready();
public:
  bool isReady() const;

  AimlBot *bot() const;
  void setBot(AimlBot *bot); // Emit ready() if bot is valid.

signals:
  void said(const QString &reply);

public:
  // CHECKPOINT: implement async!
  void chat(const QString &text, bool async = true);

};

#endif // DOLL_H
