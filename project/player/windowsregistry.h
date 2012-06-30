#ifndef WINDOWSREGISTRY_H
#define WINDOWSREGISTRY_H

// windowsregistry.h
// 4/21/2012

#include <QtCore/QObject>
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QSettings)

class WindowsRegistry : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(WindowsRegistry)
  typedef WindowsRegistry Self;
  typedef QObject Base;

  QSettings *classes_;

public:
  static Self *globalInstance();

  explicit WindowsRegistry(QObject *parent = 0)
    : Base(parent), classes_(0) { }

  void setClassesRoot(const QString &regpath);

  bool isValid() const { return classes_; }

public slots:
  void sync();

  // - Type -
public:
  bool containsType(const QString &type) const
  { return containsRawType(rawTypeOf(type)); }
  void registerTypes(const QStringList &types)
  { foreach (const QString &t, types) registerType(t); }
  void unregisterTypes(const QStringList &types)
  { foreach (const QString &t, types) unregisterType(t); }
public slots:
  void registerType(const QString &type)
  { registerRawType(rawTypeOf(type)); }
  void unregisterType(const QString &type)
  { unregisterRawType(rawTypeOf(type)); }
  void registerType(const QString &type, bool t)
  { if (t) registerType(type); else unregisterType(type); }

  // - Raw Type -
public:
  bool containsRawType(const QString &suffix) const;
  void registerRawTypes(const QStringList &suffices)
  { foreach (const QString &s, suffices) registerRawType(s); }
  void unregisterRawTypes(const QStringList &suffices)
  { foreach (const QString &s, suffices) unregisterRawType(s); }
public slots:
  void registerRawType(const QString &suffix);
  void unregisterRawType(const QString &suffix);
  void registerRawType(const QString &suffix, bool t)
  { if (t) registerRawType(suffix); else unregisterRawType(suffix); }

  // - Shell -
public:
  bool containsShell(const QString &type) const;
  void registerShells(const QStringList &types)
  { foreach (const QString &s, types) registerShell(s); }
  void unregisterShells(const QStringList &types)
  { foreach (const QString &s, types) unregisterShell(s); }
public slots:
  void registerShell(const QString &type);
  void unregisterShell(const QString &type);
  void registerShell(const QString &type, bool t)
  { if (t) registerShell(type); else unregisterShell(type); }

protected:
  static QString rawTypeOf(const QString &t)
  { return t.startsWith('.') ? t : QString(t).prepend('.'); }

  static QString aliasOf(const QString &t);

  //void clearFileTypes();
};

#endif // WINDOWSREGISTRY_H
