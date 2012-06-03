#ifndef ASSOCIATIONPREFS_H
#define ASSOCIATIONPREFS_H

// associationprefs.h
// 5/25/2012

#include "project/common/acpreferencestab.h"
#include <QtCore/QHash>
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QLineEdit)

class WindowsRegistry;
class AssociationPreferencesTab : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AssociationPreferencesTab)
  typedef AssociationPreferencesTab Self;
  typedef AcPreferencesTab Base;

  QStringList supportedSuffices_;
  WindowsRegistry *registry_;
  QHash<QString, QCheckBox *> toggles_;
  QLineEdit *searchEdit_;

public:
  explicit AssociationPreferencesTab(WindowsRegistry *registry, QWidget *parent = 0)
    : Base(parent), registry_(registry) { init(); }

  explicit AssociationPreferencesTab(QWidget *parent = 0)
    : Base(parent), registry_(0) { init(); }

public slots:
  virtual bool save(); ///< \override
  virtual void load(); ///< \override
  virtual void clear(); ///< \override
protected slots:
  void filterToggles(const QString &regexp);
  void toggleType(bool checked, const QString &type);
private:
  void init();
  void createLayout();
};

#endif // ASSOCIATIONPREFS_H
