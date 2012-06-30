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

  int language_,
      script_;

protected:
  typedef QList<QTranslator *> QTranslatorList;

  explicit TranslatorManager(QObject *parent = 0)
    : Base(parent), language_(0), script_(0) { }
public:
  static Self *globalInstance() { static Self g; return &g; }
public:
  int language() const { return language_; }
  int script() const { return script_; }
  void setLocale(int language, int script = 0, bool updateTranslator = true); ///< if true, app translator is updated automatically

signals:
  void localeChanged(); // Use this signal instead of LanguageChangeEvent to avoid refresh tr twice for replacing tr

public:
  QString translate(int id) const;

  void installCurrentTranslator(QCoreApplication *a);
  void removeCurrentTranslator(QCoreApplication *a);

protected:
  const QTranslatorList &tr_en() const;
  const QTranslatorList &tr_ja() const;
  const QTranslatorList &tr_zh_TW() const;
  const QTranslatorList &tr_zh_CN() const;

  QTranslatorList currentTranslators() const;

private:
  mutable QTranslatorList tr_en_,
                          tr_ja_,
                          tr_zh_TW_,
                          tr_zh_CN_;
};

#endif // TRANSLATORMANAGER_H
