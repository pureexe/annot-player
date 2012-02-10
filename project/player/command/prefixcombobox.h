#ifndef PREFIXCOMBOBOX_H
#define PREFIXCOMBOBOX_H

// prefixcombobox.h
// 9/26/2011

#include "annotationcomboedit.h"
#include <QVector>

class PrefixComboBox : public AnnotationComboEdit
{
  Q_OBJECT
  typedef PrefixComboBox Self;
  typedef AnnotationComboEdit Base;

public:
  explicit PrefixComboBox(QWidget *parent = 0);
};

#endif // PREFIXCOMBOBOX_H
