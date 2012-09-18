#ifndef QXEXTEDIT_H
#define QXTEXTEDIT_H

// qxtextedit.h
// 6/2/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QTextEdit>

QTX_BEGIN_NAMESPACE

class QxTextEdit : public QTextEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(QxTextEdit)
  typedef QxTextEdit Self;
  typedef QTextEdit Base;

public:
  explicit QxTextEdit(QWidget *parent = nullptr)
    :  Base(parent) { }

  explicit QxTextEdit(const QString &text, QWidget *parent = nullptr)
    :  Base(text, parent) { }

public slots:
  void removeLastLine();
};

QTX_END_NAMESPACE

#endif // QXTEXTEDIT_H
