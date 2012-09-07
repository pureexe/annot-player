#ifndef MECABPARSER_H
#define MECABPARSER_H

// 9/3/2012
// mecabparser.h

#include <QtCore/QObject>
#include <QtCore/QString>

namespace MeCab { class Tagger; }

class MeCabParser : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(MeCabParser)
  typedef MeCabParser Self;
  typedef QObject Base;

  mutable MeCab::Tagger *tagger_;

public:
  enum RenderHint {
    NoHint = 0,
    ResizeHint = 1,
    ColorHint = 1 << 1
  };

  explicit MeCabParser(QObject *parent = nullptr)
    : Base(parent), tagger_(0) { }
  ~MeCabParser();

  //QString renderText(const QString &text);
  QString renderTextWithFurigana(const QString &text, ulong hints = 0);

  // - Properties -
protected:
  MeCab::Tagger *tagger() const;
};

#endif // MECABPARSER_H
