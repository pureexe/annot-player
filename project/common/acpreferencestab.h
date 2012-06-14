#ifndef ACPREFERENCESTAB_H
#define ACPREFERENCESTAB_H

// acpreferencestab.h
// 5/5/2012

#include <QtGui/QWidget>

class AcSettings;
class AcPreferencesTab : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(AcPreferencesTab)
  typedef AcPreferencesTab Self;
  typedef QWidget Base;

  AcSettings *settings_;

public:
  explicit AcPreferencesTab(AcSettings *settings, QWidget *parent = 0)
    : Base(parent), settings_(settings) { }

  explicit AcPreferencesTab(QWidget *parent = 0)
    : Base(parent), settings_(0) { }

signals:
  void message(const QString &text);
  void warning(const QString &text);
  void error(const QString &text);
  void notification(const QString &text);

protected:
  AcSettings *settings() const { return settings_; }

public slots:
  virtual void clear() { }
  virtual bool save() = 0;
  virtual void load() = 0;
};

class AcNullPreferencesTab : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcNullPreferencesTab)
  typedef AcNullPreferencesTab Self;
  typedef AcPreferencesTab Base;

public:
  explicit AcNullPreferencesTab(QWidget *parent = 0)
    : Base(parent) { }

public slots:
  virtual bool save() { return true; } ///< \reimp
  virtual void load() { } ///< \reimp
};

#endif // ACPREFERENCESTAB_H
