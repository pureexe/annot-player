// registry.cc
// 4/21/2012

#include "registry.h"
#include "global.h"
#include "tr.h"
#ifdef Q_OS_WIN
# include "win/reg/regdefs.h"
#else
# error "require Windows"
#endif // Q_OS_WIN
#include <QtCore>

#define APPID   G_ORGANIZATION "." G_APPLICATION

//#define DEBUG "registry"
#include "module/debug/debug.h"

// - Construction -

Registry*
Registry::globalInstance()
{
  static Self g;
  g.setClassesRoot(REG_HKCU_CLASSES);
  return &g;
}

void
Registry::setClassesRoot(const QString &regpath)
{
  if (classes_)
    delete classes_;
  classes_ = new QSettings(regpath, QSettings::NativeFormat, this);
}

void
Registry::sync()
{ if (classes_) classes_->sync(); }

// - Raw Types -

QString
Registry::aliasOf(const QString &t)
{
  //QString alias = APPID + '.' + type;
  QString ret = t;
  if (!t.startsWith('.'))
    ret.prepend('.');
  ret.prepend(APPID);
  return ret;
}

bool
Registry::containsRawType(const QString &type) const
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
Registry::registerRawType(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;

  static QString command;
  if (command.isEmpty()) {
    QString app = QCoreApplication::applicationFilePath();
    app = QDir::toNativeSeparators(app);
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
Registry::unregisterRawType(const QString &type)
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

// - Shells -

bool
Registry::containsShell(const QString &type) const
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return false;

  QString shellCommand = type + "/shell/playWithAnnotPlayer/command/." ;
  bool ret = classes_->contains(shellCommand);
  DOUT("ret =" << ret);
  return ret;
}

void
Registry::registerShell(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;

  static QString command;
  if (command.isEmpty()) {
    QString app = QCoreApplication::applicationFilePath();
    app = QDir::toNativeSeparators(app);
    command = "\"" + app  + "\" %1"; // NOTE: %1 is not quoted, as DVD/AudioCD won't strip off quotes
  }

  QString text = tr("Play with Annot Player");
  classes_->beginGroup(type);
  classes_->setValue("shell/playWithAnnotPlayer/.", text);
  classes_->setValue("shell/playWithAnnotPlayer/command/.", command);
  classes_->endGroup();
  DOUT("status =" << classes_->status());
}

void
Registry::unregisterShell(const QString &type)
{
  Q_ASSERT(isValid());
  DOUT("type =" << type);

  if (!classes_ || type.isEmpty())
    return;

  QString shell = type + "/shell/playWithAnnotPlayer" ;
  classes_->remove(shell);

  DOUT("status =" << classes_->status());
}
// EOF
