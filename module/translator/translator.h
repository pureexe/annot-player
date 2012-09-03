#ifndef TRANSLATOR_H
#define TRANSLATOR_H

// translator.h
// 11/2/2011

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>

class Translator : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Translator)
  typedef Translator Self;
  typedef QObject Base;

  bool enabled_;
  bool synchronized_;

  QHash<QString, QString> cache_;

  // - Constructions -
public:
  ///  Return lcode, usually 2 letters
  static QString languageCode(int language, int script = 0);

public:
  explicit Translator(QObject *parent = nullptr)
    : Base(parent), enabled_(true), synchronized_(true) { }

  bool isEnabled() const { return enabled_; }
  bool isSynchronized() const { return synchronized_; }

  virtual QString name() const = 0;

signals:
  void errorMessage(const QString &msg);
  void translated(const QString &text); ///< Requested translation received
  void enabledChanged(bool t);
  void synchronizedChanged(bool t);

public slots:
  virtual void translate(const QString &text, const QString &to, const QString &from = QString());

  void setEnabled(bool t)
  {
    if (enabled_ != t) {
      emit enabledChanged(enabled_ = t);
      if (!enabled_)
        clearCache();
    }
  }

  void setSynchronized(bool t)
  { if (synchronized_ != t) emit synchronizedChanged(synchronized_ = t); }

  void clearCache() { if (!cache_.isEmpty()) cache_.clear(); }

protected:
  virtual void doTranslate(const QString &text, const QString &to, const QString &from) = 0;

  void cacheTranslation(const QString &s, const QString &t) { cache_[s] = t; }
  const QHash<QString, QString> &cachedTranslation() { return cache_; }
  //QHash<QString, QString> &cachedTranslation() { return cache_; }
};

#endif // TRANSLATOR_H
