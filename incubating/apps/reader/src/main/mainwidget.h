#ifndef MAINWIDGET_H
#define MAINWIDGET_H

// mainwidget.h
// 9/22/2012 jichi

#include "qx/qxglobal.h"
#include <QtWidgets/QWidget>

class MainWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(MainWidget)
  QX_EXTEND_CLASS(MainWidget, QWidget)

public:
  MainWidget();
};

#endif // MAINWIDGET_H
