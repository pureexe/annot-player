#ifndef ASSOCIATIONPREFS_H
#define ASSOCIATIONPREFS_H

// associationprefs.h
// 5/25/2012

#include "src/common/acpreferencestab.h"
#include <QtCore/QHash>
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QLineEdit)

class Registry;
class AssociationPreferencesTab : public AcPreferencesTab
{
  Q_OBJECT
  Q_DISABLE_COPY(AssociationPreferencesTab)
  typedef AssociationPreferencesTab Self;
  typedef AcPreferencesTab Base;

  QStringList supportedTypes_,
              supportedShells_;
  Registry *registry_;
  QHash<QString, QCheckBox *> toggles_;
  QLineEdit *searchEdit_;

public:
  explicit AssociationPreferencesTab(Registry *registry, QWidget *parent = nullptr)
    : Base(parent), registry_(registry) { init(); }

  explicit AssociationPreferencesTab(QWidget *parent = nullptr)
    : Base(parent), registry_(nullptr) { init(); }

public slots:
  bool save() override;
  void load() override;
  void clear() override;
protected slots:
  void filterToggles(const QString &regexp);
  void toggleType(bool checked, const QString &type);
  void toggleShell(bool checked, const QString &type);
private:
  void init();
  void createLayout();
};

#endif // ASSOCIATIONPREFS_H
