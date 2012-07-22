#ifndef TRANSLATORMANAGER_H
#define TRANSLATORMANAGER_H

// translatormanager.h
// 7/1/2012

#include "module/translator/translator.h"
#include <QtCore/QUrl>

class TranslatorManager : public Translator
{
  Q_OBJECT
  Q_DISABLE_COPY(TranslatorManager)
  typedef TranslatorManager Self;
  typedef Translator Base;

  // - Constructions -
public:
  enum ServiceProvider { Microsoft = 0, Google, Romaji, ServiceCount};
  enum ServiceProviderBit { MicrosoftBit = 1 << Microsoft, GoogleBit = 1 << Google, RomajiBit = 1 << Romaji };
  enum { AllServices = MicrosoftBit | GoogleBit | RomajiBit };
public:
  explicit TranslatorManager(QObject *parent = nullptr);

  ulong services() const { return services_; }
  bool hasServices() const { return services_; }
  bool hasService(int service) const { return services_ & (1 << service); }

  int serviceCount() const
  {
    int ret = 0;
    for (int i = 0; i < ServiceCount; i++)
      if (hasService(i))
        ret++;
    return ret;
  }

signals:
  void servicesChanged(ulong mask);

  void translatedByMicrosoft(const QString &text);
  void translatedByGoogle(const QString &text);
  void translatedByRomaji(const QString &text);

public slots:
  void setServices(ulong mask)
  { if (services_ != mask) emit servicesChanged(services_ = mask); }

  void setService(int service, bool enable)
  {
    setServices(enable ?
      services_ | (1 << service) :
      services_ & ~(1 << service)
    );
  }

  void translate(const QString &text, const QString &to, const QString &from = QString()) override
  {
    for (int service = 0; service < ServiceCount; service++)
      if (hasService(service))
        translate(service, text, to, from);
  }

  void translate(int service, const QString &text, const QString &to, const QString &from = QString())
  {
    Q_ASSERT(service >= 0 && service < ServiceCount);
    t_[service]->translate(text, to, from);
  }

private:
  Translator *t_[ServiceCount];
  ulong services_;
};

#endif // BINGTRANSLATOR_H
