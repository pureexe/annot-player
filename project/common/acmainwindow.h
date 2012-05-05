#ifndef ACMAINWINDOW_H
#define ACMAINWINDOW_H

// acmainwindow.h
// 2/17/2012

#include "module/qtext/draggablemainwindow.h"

QT_FORWARD_DECLARE_CLASS(QTimer)

typedef QtExt::DraggableMainWindow AcMainWindowBase;
class AcMainWindow : public AcMainWindowBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcMainWindow)
  typedef AcMainWindow Self;
  typedef AcMainWindowBase Base;

  // - Construction -
public:
  explicit AcMainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);

  //-  Log -
signals:
  void message(const QString &text);
  void errorMessage(const QString &text);
  void warning(const QString &text);
  void notification(const QString &text);

protected slots:
  void showMessage(const QString &text);
  void error(const QString &text);
  void warn(const QString &text);
  void notify(const QString &text);

protected:
  bool autoHideMenuBar() const { return autoHideMenuBar_; }
protected slots:
  void setAutoHideMenuBar(bool t) { autoHideMenuBar_ = t; }

private:
  QTimer *messageTimer_;
  bool autoHideMenuBar_;
};

#endif // ACMAINWINDOW_H
