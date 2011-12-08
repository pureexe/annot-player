// core/gui/actionwithid.cc
// 9/2/2011

#include "core/gui/actionwithid.h"
#include <QtGui>

// - Constructions -

#define CONSTRUCTOR \
  { createConnections(); }

Core::Gui::
ActionWithId::ActionWithId(QObject *parent)
  : Base(parent), id_(0)
CONSTRUCTOR

Core::Gui::
ActionWithId::ActionWithId(const QString &text, QObject *parent)
  : Base(text, parent), id_(0)
CONSTRUCTOR

Core::Gui::
ActionWithId::ActionWithId(const QIcon &icon, const QString &text, QObject *parent)
  : Base(icon, text, parent), id_(0)
CONSTRUCTOR

Core::Gui::
ActionWithId::ActionWithId(int id, QObject *parent)
  : Base(parent), id_(id)
CONSTRUCTOR

Core::Gui::
ActionWithId::ActionWithId(int id, const QString &text, QObject *parent)
  : Base(text, parent), id_(id)
CONSTRUCTOR

Core::Gui::
ActionWithId::ActionWithId(int id, const QIcon &icon, const QString &text, QObject *parent)
  : Base(icon, text, parent), id_(id)
CONSTRUCTOR

#undef CONSTRUCTOR

void
Core::Gui::
ActionWithId::createConnections()
{
  connect(this, SIGNAL(changed()), SLOT(emit_changedWithId()));
  connect(this, SIGNAL(hovered()), SLOT(emit_hoveredWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
  connect(this, SIGNAL(triggered(bool)), SLOT(emit_triggeredWithId(bool)));
}

// - Properties -

int
Core::Gui::
ActionWithId::id() const
{ return id_; }

void
Core::Gui::
ActionWithId::setId(int id)
{ id_ = id; }

// - Emits -

void
Core::Gui::
ActionWithId::emit_changedWithId()
{ emit changedWithId(id_); }

void
Core::Gui::
ActionWithId::emit_hoveredWithId()
{ emit hoveredWithId(id_); }

void
Core::Gui::
ActionWithId::emit_toggledWithId(bool checked)
{ emit toggledWithId(id_, checked); }

void
Core::Gui::
ActionWithId::emit_triggeredWithId(bool checked)
{ emit triggeredWithId(id_, checked); }

// EOF
