#ifndef ACWINDOW_H
#define ACWINDOW_H

// backlogview.h
// 2/18/2012

#include "module/qtext/draggablewidget.h"
#include <QtCore/QList>

class AcTextView;
typedef QtExt::DraggableWidget AcWindowBase;
class AcWindow : public AcWindowBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcWindow)
  typedef AcWindow Self;
  typedef AcWindowBase Base;

public:
  explicit AcWindow(QWidget *parent = 0);
};

#endif // ACWINDOW_H
