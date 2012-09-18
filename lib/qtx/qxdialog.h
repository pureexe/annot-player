#ifndef QXDIALOG_H
#define QXDIALOG_H

// qtext/dialog.h
// 9/3/2011

#include "qtx/qxdraggablewidget.h"

#define QTX_DIALOG_WINDOW_FLAGS \
  (Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)

QTX_BEGIN_NAMESPACE

class QxDialog : public QxDraggableWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDialog)
  typedef QxDialog Self;
  typedef QxDraggableWidget Base;

public:
  ///  Default f: Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
  explicit QxDialog(QWidget *parent = nullptr, Qt::WindowFlags f = 0)
    : Base(parent, f ? f : QTX_DIALOG_WINDOW_FLAGS) { }
};

QTX_END_NAMESPACE

#endif // QXDIALOG_H
