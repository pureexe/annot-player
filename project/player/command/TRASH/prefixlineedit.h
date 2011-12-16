#ifndef PREFIXLINEEDIT_H
#define PREFIXLINEEDIT_H

// prefixlineedit.h
// 9/26/2011

#include "commandlineedit.h"
#include <QVector>

class PrefixLineEdit : public CommandLineEdit
{
  Q_OBJECT
  typedef PrefixLineEdit Self;
  typedef CommandLineEdit Base;

public:
  explicit PrefixLineEdit(QWidget *parent = 0);
};

#endif // PREFIXLINEEDIT_H
