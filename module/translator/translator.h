#ifndef TRANSLATOR_H
#define TRANSLATOR_H

// translator.h
// 11/2/2011

#include <QtCore/QObject>

class Translator : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Translator)
  typedef Translator Self;
  typedef QObject Base;

  // - Constructions -
public:
  ///  Return lcode, usually 2 letters
  static QString languageCode(int language, int script = 0);

public:
  explicit Translator(QObject *parent = 0) : Base(parent) { }

signals:
  void error(const QString &msg);
  void translated(const QString &text); ///< Requested translation received

public slots:
  virtual void translate(const QString &text, const QString &to, const QString &from = QString()) = 0;
};

#endif // TRANSLATOR_H
