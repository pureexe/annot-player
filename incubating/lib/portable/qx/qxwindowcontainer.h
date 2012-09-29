#ifndef QXWINDOWCONTAINER_H
#define QXWINDOWCONTAINER_H

// qxwindowcontainer.h
// 9/23/2012

#include "qx/qxglobal.h"
#include <QtWidgets/QWidget>

QT_FORWARD_DECLARE_CLASS(QWindow)

QX_BEGIN_NAMESPACE

///  Take the ownership of the specified delegate window
class QxWindowContainer : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(QxWindowContainer)
  QX_EXTEND_CLASS(QxWindowContainer, QWidget)

  QWindow *delegate_;

public:
  explicit QxWindowContainer(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  explicit QxWindowContainer(QWindow *delegate, QWidget *parent = nullptr, Qt::WindowFlags f = 0);

  // - Properties -
public:
  QWindow *delegateWindow() const;
public slots:
  void setDelegateWindow(QWindow *window);

  // - Events -
public slots:
  void setVisible(bool visible) override;
public:
  bool eventFilter(QObject *obj, QEvent *ev);
protected:
  bool event(QEvent *ev);

private:
  void init();
};

QX_END_NAMESPACE

#endif // QXWINDOWCONTAINER_H
