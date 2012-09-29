#ifndef QXDRAGGABLEDILOG_H
#define QXDRAGGABLEDILOG_H

// qxdraggabledialog.h
// 9/3/2011

#include "qx/qxglobal.h"
#include <QtWidgets/QDialog>

QX_BEGIN_NAMESPACE

class QxDraggableDialogPrivate;
class QxDraggableDialog : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDraggableDialog)
  QX_EXTEND_CLASS(QxDraggableDialog, QDialog)
  QX_DECLARE_PRIVATE(QxDraggableDialogPrivate)

public:
  explicit QxDraggableDialog(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
  ~QxDraggableDialog();

  // - Properties -
public:
  bool isDraggable() const;
public slots:
  void setDraggable(bool value);

  // - Events -
protected:
  //void setVisible(bool visible) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
};

QX_END_NAMESPACE

#endif // QXDRAGGABLEDILOG_H
