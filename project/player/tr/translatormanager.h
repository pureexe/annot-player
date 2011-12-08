#ifndef TRANSLATORMANAGER_H
#define TRANSLATORMANAGER_H

// translatormanager.h
// 8/7/2011

#include <QObject>
#include <QLocale>

QT_FORWARD_DECLARE_CLASS(QTranslator)
QT_FORWARD_DECLARE_CLASS(QCoreApplication)

class TranslatorManager : public QObject
{
  Q_OBJECT
  typedef TranslatorManager Self;
  typedef QObject Base;

  static int language_;

public:
  static Self *globalInstance();
public:
  int language() const;
  void setLanguage(int language, bool updateTranslator = true); ///< if true, app translator is updated automatically

  enum { TraditionalChinese = -1 }; // TO BE REMOVED AFTER Qt 4.8

signals:
  void languageChanged(); // Use this signal instead of LanguageChangeEvent to avoid refresh tr twice for replacing tr

public:
  QString translate(int id) const;

  void installCurrentTranslator(QCoreApplication *a);
  void removeCurrentTranslator(QCoreApplication *a);

protected:
  explicit TranslatorManager(QObject *parent = 0);

protected:
  QTranslator *tr_en() const;
  QTranslator *tr_ja() const;
  QTranslator *tr_tw() const;
  QTranslator *tr_zh() const;

  QTranslator *currentTranslatorManager() const;

private:
  mutable QTranslator *tr_en_, *tr_ja_, *tr_tw_, *tr_zh_;
};

#endif // TRANSLATORMANAGER_H
