#ifndef PREFIXCOMBOBOX_H
#define PREFIXCOMBOBOX_H

// prefixcombobox.h
// 9/26/2011

#include "annotationcomboedit.h"

class PrefixComboBox : public AnnotationComboEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(PrefixComboBox)
  typedef PrefixComboBox Self;
  typedef AnnotationComboEdit Base;

public:
  explicit PrefixComboBox(QWidget *parent = nullptr);
};

#endif // PREFIXCOMBOBOX_H
