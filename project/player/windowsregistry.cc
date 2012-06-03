// windowsregistry.cc
// 4/21/2012

#include "windowsregistry.h"
#include "global.h"
#include "tr.h"
#ifdef Q_WS_WIN
#  include "win/qtwin/winreg.h"
#else
#  error "Windows only"
#endif // Q_WS_WIN
#include <QtCore>

#define APPID   G_ORGANIZATION "." G_APPLICATION

//#define DEBUG "windowsregistry"
#include "module/debug/debug.h"

// - Construction -

WindowsRegistry*
WindowsRegistry::globalInstance()
{
  static Self g;
  g.setClassesRoot(REG_HKCU_SOFTWARE_CLASSES);
  return &g;
}

void
WindowsRegistry::setClassesRoot(const QString &regpath)
{
  if (classes_)
    delete classes_;
  classes_ = new QSettings(regpath, QSettings::NativeFormat, this);
}

void
WindowsRegistry::sync()
{ if (classes_) classes_->sync(); }

// - Register -

void
WindowsRegistry::registerTypes(const QStringList &suffices)
{
  DOUT("enter: suffices =" << suffices);
  foreach (const QString &suffix, suffices)
    registerType(suffix);
  DOUT("exit");
}

bool
WindowsRegistry::containsType(const QString &type) const
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return false;
  QString suffix = type;
  if (!suffix.startsWith('.'))
    suffix.prepend('.');

  QString alias = APPID + suffix;
  QString aliasId = suffix + "/OpenWithProgIds/" + alias;
  bool ret = classes_->contains(aliasId);
  DOUT("ret =" << ret);
  return ret;
}

void
WindowsRegistry::registerType(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;
  QString suffix = type;
  if (!suffix.startsWith('.'))
    suffix.prepend('.');

  static QString command;
  if (command.isEmpty()) {
    QString app = QCoreApplication::applicationFilePath();
    app.replace('/', '\\');
    command = "\"" + app  + "\" \"%1\"";
  }

  QString alias = APPID + suffix;

  classes_->beginGroup(suffix);

  classes_->setValue("OpenWithProgIds/" + alias, "");
  classes_->endGroup();

  QString icon; // TODO
  QString description = alias; // TODO
  QString open = TR(T_PLAY);

  DOUT("alias =" << alias << ", command =" << command);

  classes_->beginGroup(alias);
  classes_->setValue(".", description);
  classes_->setValue("DefaultIcon/.", icon);
  classes_->setValue("shell/.", "open");
  classes_->setValue("shell/open/.", open);
  classes_->setValue("shell/open/command/.", command);
  classes_->endGroup();

  DOUT("status =" << classes_->status());
}

void
WindowsRegistry::unregisterType(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;
  QString suffix = type;
  if (!suffix.startsWith('.'))
    suffix.prepend('.');

  QString alias = APPID + suffix;
  QString aliasId = suffix + "/OpenWithProgIds/" + alias;

  classes_->remove(aliasId);
  classes_->remove(alias);

  DOUT("status =" << classes_->status());
}

// EOF
