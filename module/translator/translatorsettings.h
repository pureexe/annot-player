#ifndef TRANSLATORSETTINGS_H
#define TRANSLATORSETTINGS_H

// translatorsettings.h
// 7/1/2012

#include <QtCore/QObject>

class TranslatorSettings : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslatorSettings)
  typedef TranslatorSettings Self;
  typedef QObject Base;

  QString cacheDirectory_;
  //bool synchronized_;

  // - Constructions -
public:
  static Self *globalSettings() { static Self g; return &g; }
protected:
  explicit TranslatorSettings(QObject *parent = nullptr)
    : Base(parent) { }

  // - Properties -
public:
  const QString &cacheDirectory() const { return cacheDirectory_; }
  bool hasCacheDirectory() const { return !cacheDirectory_.isEmpty(); }
  void setCacheDirectory(const QString &path) { cacheDirectory_ = path; }

  //bool isSynchronized() const { return synchronized_; }
  //void setSynchronized(bool t) { synchronized_ = t; }
};

#endif // TRANSLATORSETTINGS_H
