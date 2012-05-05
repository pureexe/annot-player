#ifndef INPUTCOMBOBOX_H
#define INPUTCOMBOBOX_H

// inputcombobox.h
// 9/26/2011

#include "annotationcomboedit.h"

class InputComboBox : public AnnotationComboEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(InputComboBox)
  typedef InputComboBox Self;
  typedef AnnotationComboEdit Base;

public:
  explicit InputComboBox(QWidget *parent = 0);
};

#endif // INPUTCOMBOBOX_H
