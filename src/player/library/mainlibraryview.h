#ifndef MAINLIBRARYVIEW_H
#define MAINLIBRARYVIEW_H

// mainlibraryview.h
// 8/17/2012

#include "medialibraryview.h"
#include <QtGui/QDockWidget>

class MainLibraryDock : public QDockWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(MainLibraryDock)
  typedef MainLibraryDock Self;
  typedef QDockWidget Base;

public:
  explicit MainLibraryDock(QWidget *parent = nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
};

class MainLibraryView : public MediaLibraryView
{
  Q_OBJECT
  Q_DISABLE_COPY(MainLibraryView)
  typedef MainLibraryView Self;
  typedef MediaLibraryView Base;

public:
  explicit MainLibraryView(MediaLibrary *library, QWidget *parent = nullptr);

  QSize sizeHint() const override;
};

#endif // MAINLIBRARYVIEW_H
