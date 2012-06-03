#ifndef WINDOWSREGISTRY_H
#define WINDOWSREGISTRY_H

// windowsregistry.h
// 4/21/2012

#include <QtCore/QObject>

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

public:
  void registerTypes(const QStringList &suffices);
  bool containsType(const QString &suffix) const;

public slots:
  void registerType(const QString &suffix);
  void unregisterType(const QString &suffix);

  void registerType(const QString &suffix, bool t)
  { if (t) registerType(suffix); else unregisterType(suffix); }

  //void clearFileTypes();
};

#endif // WINDOWSREGISTRY_H
