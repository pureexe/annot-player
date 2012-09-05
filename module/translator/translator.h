#ifndef TRANSLATOR_H
#define TRANSLATOR_H

// translator.h
// 11/2/2011

#include "module/mstypes/lcid.h"
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

  // - Types -
public:
  enum Language {
    NoLanguage = 0,
    English = LCID_EN_US,
    Japanese = LCID_JA_JP,
    TraditionalChinese = LCID_ZH_TW,
    SimplifiedChinese = LCID_ZH_CN,
    Korean = LCID_KO_KR,
    French = LCID_FR,
    German = LCID_DE,
    Italian = LCID_IT,
    Spanish = LCID_ES,
    Portuguese = LCID_PT,
    Russian = LCID_RU
  };

  static bool isChineseLanguage(int lcid)
  { return lcid == TraditionalChinese || lcid == SimplifiedChinese; }

  // - Construction -
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
  ///  \param from and \param to are supposed in Microsoft LCID format.
  virtual void translate(const QString &text, int to, int from = 0);

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
  virtual void doTranslate(const QString &text, int to, int from) = 0;

  void cacheTranslation(const QString &s, const QString &t) { cache_[s] = t; }
  const QHash<QString, QString> &cachedTranslation() { return cache_; }
  //QHash<QString, QString> &cachedTranslation() { return cache_; }
};

#endif // TRANSLATOR_H
