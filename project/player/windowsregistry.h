#ifndef WINDOWSREGISTRY_H
#define WINDOWSREGISTRY_H

// windowsregistry.h
// 4/21/2012

#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QSettings)

class WindowsRegistry : public QObject
{
  Q_OBJECT
  typedef WindowsRegistry Self;
  typedef QObject Base;

  QSettings *classes_;

public:
  explicit WindowsRegistry(QObject *parent = 0)
    : Base(parent), classes_(0) { }

  void setClassesRoot(const QString &regpath);

  bool isValid() const { return classes_; }

  void sync();

public:
  void registerFileTypes(const QStringList &suffices);
  void registerFileType(const QString &suffix);

  //void clearFileTypes();
};

#endif // WINDOWSREGISTRY_H
