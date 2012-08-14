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

  bool enabled_;

  // - Constructions -
public:
  ///  Return lcode, usually 2 letters
  static QString languageCode(int language, int script = 0);

public:
  explicit Translator(QObject *parent = nullptr)
    : Base(parent), enabled_(true) { }

  bool isEnabled() const { return enabled_; }

signals:
  void errorMessage(const QString &msg);
  void translated(const QString &text); ///< Requested translation received
  void enabledChanged(bool t);

public slots:
  virtual void translate(const QString &text, const QString &to, const QString &from = QString()) = 0;

  void setEnabled(bool t)
  { if (enabled_ != t) emit enabledChanged(enabled_ = t); }
};

#endif // TRANSLATOR_H
