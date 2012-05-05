#ifndef TRANSLATORMANAGER_H
#define TRANSLATORMANAGER_H

// translatormanager.h
// 8/7/2011

#include <QObject>
#include <QLocale>
#include <QList>

QT_FORWARD_DECLARE_CLASS(QTranslator)
QT_FORWARD_DECLARE_CLASS(QCoreApplication)

class TranslatorManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslatorManager)
  typedef TranslatorManager Self;
  typedef QObject Base;

  enum { NoLanguage = 0 };
  int language_;

protected:
  typedef QList<QTranslator *> QTranslatorList;

  explicit TranslatorManager(QObject *parent = 0)
    : Base(parent), language_(NoLanguage) { }
public:
  static Self *globalInstance() { static Self g; return &g; }
public:
  int language() const { return language_; }
  void setLanguage(int language, bool updateTranslator = true); ///< if true, app translator is updated automatically

  enum { TraditionalChinese = -1 }; // TO BE REMOVED AFTER Qt 4.8

signals:
  void languageChanged(); // Use this signal instead of LanguageChangeEvent to avoid refresh tr twice for replacing tr

public:
  QString translate(int id) const;

  void installCurrentTranslator(QCoreApplication *a);
  void removeCurrentTranslator(QCoreApplication *a);

protected:
  const QTranslatorList &tr_en() const;
  const QTranslatorList &tr_ja() const;
  const QTranslatorList &tr_tw() const;
  const QTranslatorList &tr_zh() const;

  QTranslatorList currentTranslators() const;

private:
  mutable QTranslatorList tr_en_,
                          tr_ja_,
                          tr_zh_,
                          tr_tw_;
};

#endif // TRANSLATORMANAGER_H
