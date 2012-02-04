// qtext/actionwithid.cc
// 9/2/2011

#include "actionwithid.h"
#include <QtGui>

// - Constructions -

#define CONSTRUCTOR \
  { createConnections(); }

QtExt::
ActionWithId::ActionWithId(QObject *parent)
  : Base(parent), id_(0)
CONSTRUCTOR

QtExt::
ActionWithId::ActionWithId(const QString &text, QObject *parent)
  : Base(text, parent), id_(0)
CONSTRUCTOR

QtExt::
ActionWithId::ActionWithId(const QIcon &icon, const QString &text, QObject *parent)
  : Base(icon, text, parent), id_(0)
CONSTRUCTOR

QtExt::
ActionWithId::ActionWithId(int id, QObject *parent)
  : Base(parent), id_(id)
CONSTRUCTOR

QtExt::
ActionWithId::ActionWithId(int id, const QString &text, QObject *parent)
  : Base(text, parent), id_(id)
CONSTRUCTOR

QtExt::
ActionWithId::ActionWithId(int id, const QIcon &icon, const QString &text, QObject *parent)
  : Base(icon, text, parent), id_(id)
CONSTRUCTOR

#undef CONSTRUCTOR

void
QtExt::
ActionWithId::createConnections()
{
  connect(this, SIGNAL(changed()), SLOT(emit_changedWithId()));
  connect(this, SIGNAL(hovered()), SLOT(emit_hoveredWithId()));
  connect(this, SIGNAL(toggled(bool)), SLOT(emit_toggledWithId(bool)));
  connect(this, SIGNAL(triggered(bool)), SLOT(emit_triggeredWithId(bool)));
}

// - Properties -

int
QtExt::
ActionWithId::id() const
{ return id_; }

void
QtExt::
ActionWithId::setId(int id)
{ id_ = id; }

// - Emits -

void
QtExt::
ActionWithId::emit_changedWithId()
{ emit changedWithId(id_); }

void
QtExt::
ActionWithId::emit_hoveredWithId()
{ emit hoveredWithId(id_); }

void
QtExt::
ActionWithId::emit_toggledWithId(bool checked)
{ emit toggledWithId(id_, checked); }

void
QtExt::
ActionWithId::emit_triggeredWithId(bool checked)
{ emit triggeredWithId(id_, checked); }

// EOF
