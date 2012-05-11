#ifndef ACPREFERENCES_P_H
#define ACPREFERENCES_P_H

// acpreferences_p.h
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
    : Base(parent), settings_(settings) { Q_ASSERT(settings_); }

signals:
  void message(const QString &text);
  void warning(const QString &text);
  void error(const QString &text);
  void notification(const QString &text);

protected:
  AcSettings *settings() const { return settings_; }

public slots:
  virtual bool save() = 0;
  virtual void load() = 0;
};

class AcEmptyPreferences : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AcEmptyPreferences)
  typedef AcEmptyPreferences Self;
  typedef AcPreferencesTab Base;

public:
  explicit AcEmptyPreferences(AcSettings *settings, QWidget *parent = 0)
    : Base(settings, parent)
  { setWindowTitle("TODO"); }

public slots:
  virtual bool save() { return true; } ///< \override
  virtual void load() { } ///< \override
};

#endif // ACPREFERENCES_P_H
