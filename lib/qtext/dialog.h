#ifndef _QTEXT_DIALOG_H
#define _QTEXT_DIALOG_H

// qtext/dialog.h
// 9/3/2011

#include "lib/qtext/draggablewidget.h"

namespace QtExt {

class Dialog : public DraggableWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(Dialog)
  typedef Dialog Self;
  typedef DraggableWidget Base;

public:
  ///  Default f: Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint;
  explicit Dialog(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
};

} // namespace QtExt

#endif // _QTEXT_DIALOG_H
