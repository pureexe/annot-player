// librarydialog.cc
// 9/24/2012

#include "librarydialog.h"
#include "qml.h"
#include <QtQml/QQmlEngine>

#define DEBUG "librarydialog"
#include "qx/qxdebug.h"

// - Construction -

LibraryDialog::LibraryDialog(QWindow *parent)
  : Base(parent)
{
  setTranslucent(true);

  setSource(QUrl::fromLocalFile(QML_LIBRARY_DIALOG));

  connect(this, SIGNAL(visibleChanged(bool)), SLOT(onVisibleChanged(bool)));
  connect(engine(), SIGNAL(quit()), SLOT(hide()));
}

// - Events -

void
LibraryDialog::onVisibleChanged(bool visible)
{
  if (!visible)
    emit closed();
}

// EOF
