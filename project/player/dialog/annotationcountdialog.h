#ifndef ANNOTATIONCOUNTDIALOG_H
#define ANNOTATIONCOUNTDIALOG_H

// annotationcountdialog.h
// 2/8/2012

#include "module/qtext/dialog.h"
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

class DataManager;

typedef QtExt::Dialog AnnotationCountDialogBase;
class AnnotationCountDialog : public AnnotationCountDialogBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationCountDialog)
  typedef AnnotationCountDialog Self;
  typedef AnnotationCountDialogBase Base;

  DataManager *dm_;
  QComboBox *edit_;
  QLabel *totalCountLabel_;

public:
  explicit AnnotationCountDialog(DataManager *dm, QWidget *parent = 0);

signals:
  void countChanged(int count);

public slots:
  virtual void setVisible(bool visible); ///< \override
  void setCount(int count);
protected slots:
  void ok();
  void updateTotalCount();

private:
  void createLayout();
};

#endif // ANNOTATIONCOUNTDIALOG_H
