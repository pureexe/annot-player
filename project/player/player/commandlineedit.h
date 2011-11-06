#ifndef COMMANDLINEEDIT_H
#define COMMANDLINEEDIT_H

// commandlineedit.h
// 7/16/2011

#include "lineedit.h"
#include <QVector>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QMenu)

class AnnotationEditor;

class CommandLineEdit : public LineEdit
{
  Q_OBJECT
  typedef CommandLineEdit Self;
  typedef LineEdit Base;

public:
  explicit CommandLineEdit(QWidget *parent = 0);

  const QVector<QString> &history() const;

  // - Properties -
protected:
  AnnotationEditor *editor() const;

  // - History -
protected slots:
  void addCurrentTextToHistory();
  void previous();
  void next();
  void clearHistory();

  void edit();

  // - Events -
protected:
  virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

private:
  void createActions();

private:
  QVector<QString> history_;
  size_t current_; // [0, history_.size() -1 +1]

  AnnotationEditor *editor_;

  QMenu *contextMenu_;
  QAction *editAct_;
};

#endif // COMMANDLINEEDIT_H
