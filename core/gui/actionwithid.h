#ifndef _CORE_GUI_ACTIONWITHID_H
#define _CORE_GUI_ACTIONWITHID_H

// core/gui/actionwithid.h
// 9/2/2011

#include <QAction>

namespace Core { namespace Gui {

class ActionWithId : public QAction
{
  Q_OBJECT
  typedef ActionWithId Self;
  typedef QAction Base;

  int id_; // init to be zero

public:
  ActionWithId(QObject *parent);
  ActionWithId(const QString &text, QObject *parent);
  ActionWithId(const QIcon &icon, const QString &text, QObject *parent);

  ActionWithId(int id, QObject *parent);
  ActionWithId(int id, const QString &text, QObject *parent);
  ActionWithId(int id, const QIcon &icon, const QString &text, QObject *parent);

  int id() const;
public slots:
  void setId(int id);

signals:
  void changedWithId(int id);
  void hoveredWithId(int id);
  void toggledWithId(int id, bool checked);
  void triggeredWithId(int id, bool checked = false);

// - Implementations -
public slots:
  void emit_changedWithId(); ///< \internal
  void emit_hoveredWithId(); ///< \internal
  void emit_toggledWithId(bool checked); ///< \internal
  void emit_triggeredWithId(bool checked = false); ///< \internal

private:
  void createConnections();
};

} } // namespace Gui, Core

#endif // _CORE_GUI_ACTIONWITHID_H
