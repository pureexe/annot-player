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

// - Types -

QString
WindowsRegistry::aliasOf(const QString &t)
{
  //QString alias = APPID + '.' + type;
  QString ret = t;
  if (!t.startsWith('.'))
    ret.prepend('.');
  ret.prepend(APPID);
  return ret;
}

bool
WindowsRegistry::containsRawType(const QString &type) const
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return false;

  QString aliasId = type + "/OpenWithProgIds/" + aliasOf(type);
  bool ret = classes_->contains(aliasId);
  DOUT("ret =" << ret);
  return ret;
}

void
WindowsRegistry::registerRawType(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;

  static QString command;
  if (command.isEmpty()) {
    QString app = QCoreApplication::applicationFilePath();
    app.replace('/', '\\');
    command = "\"" + app  + "\" \"%1\"";
  }

  QString alias = aliasOf(type);

  classes_->beginGroup(type);

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
WindowsRegistry::unregisterRawType(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;

  QString alias = aliasOf(type);
  QString aliasId = type + "/OpenWithProgIds/" + alias;

  classes_->remove(aliasId);
  classes_->remove(alias);

  DOUT("status =" << classes_->status());
}

// EOF
