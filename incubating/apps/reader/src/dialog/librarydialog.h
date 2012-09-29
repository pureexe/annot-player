#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

// launcherdialog.h
// 9/24/2012
// Game launcher dialog

#include "sakurakit/skquickview.h"

class LibraryDialog : public SkQuickView
{
  Q_OBJECT
  Q_DISABLE_COPY(LibraryDialog)
  QX_EXTEND_CLASS(LibraryDialog, SkQuickView)

public:
  explicit LibraryDialog(QWindow *parent = nullptr);

signals:
  void closed();

protected slots:
  void onVisibleChanged(bool visible);
};

#endif // LIBRARYDIALOG_H
