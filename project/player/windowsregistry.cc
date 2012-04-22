// windowsregistry.cc
// 4/21/2012

#include "windowsregistry.h"
#include "global.h"
#include "tr.h"
#include <QtCore>

#define APPID   G_ORGANIZATION "." G_APPLICATION

//#define DEBUG "windowsregistry"
#include "module/debug/debug.h"

// - Construction -

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
WindowsRegistry::registerFileTypes(const QStringList &suffices)
{
  DOUT("enter: suffices =" << suffices);
  foreach (const QString &suffix, suffices)
    registerFileType(suffix);
  DOUT("exit");
}

void
WindowsRegistry::registerFileType(const QString &type)
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

// EOF
