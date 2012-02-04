#ifndef COMBOBOXEDIT_H
#define COMBOBOXEDIT_H

// comboboxedit.h
// 7/16/2011

#include "module/qtext/combobox.h"
#include "module/qtext/withsizehint.h"
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class AnnotationEditor;

typedef QtExt::ComboBox ComboBoxEditBase;
class ComboBoxEdit : public ComboBoxEditBase, public QtExt::WithSizeHint
{
  Q_OBJECT
  typedef ComboBoxEdit Self;
  typedef ComboBoxEditBase Base;

public:
  explicit ComboBoxEdit(QWidget *parent = 0);

  // - Properties -
public slots:
  void reset();
  void setDefaultItems(const QStringList &l) { defaultItems_ = l; }

  virtual QSize sizeHint() const ///< \override QWidget
  { return QtExt::WithSizeHint::sizeHint(); }

public:
  const QStringList &defaultItems() const { return defaultItems_; }
protected:
  QStringList &defaultItems() { return defaultItems_; }

  // - History -
protected slots:
  //void previous();
  //void next();

  void edit();

  // - Events -
protected:
  //virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  void createActions();

private:
  QStringList defaultItems_;

  AnnotationEditor *editor_;

  QMenu *contextMenu_;
  QAction *editAct_,
          *clearAct_;
};

#endif // COMBOBOXEDIT_H
