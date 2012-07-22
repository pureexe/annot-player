#ifndef ANNOTATIONCOMBOEDIT_H
#define ANNOTATIONCOMBOEDIT_H

// annotationcomboedit.h
// 7/16/2011

#include "project/common/accomboedit.h"
#include "module/qtext/withsizehint.h"
#include <QtCore/QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)

class AnnotationEditor;

class AnnotationComboEdit : public AcComboEdit, public QtExt::WithSizeHint
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationComboEdit)
  typedef AnnotationComboEdit Self;
  typedef AcComboEdit Base;

public:
  enum { EditAction = CustomAction };

  explicit AnnotationComboEdit(QWidget *parent = nullptr);

  // - Properties -
public slots:
  QSize sizeHint() const override
  { return QtExt::WithSizeHint::sizeHint(); }

protected slots:
  void edit();

  // - Events -
protected:
  void keyPressEvent(QKeyEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;

private:
  void createActions();

private:
  AnnotationEditor *editor_;

protected:
  QAction *editAct;
};

#endif // ANNOTATIONCOMBOEDIT_H
