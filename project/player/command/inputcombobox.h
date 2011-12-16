#ifndef INPUTCOMBOBOX_H
#define INPUTCOMBOBOX_H

// inputcombobox.h
// 9/26/2011

#include "comboboxedit.h"
#include <QVector>

class InputComboBox : public ComboBoxEdit
{
  Q_OBJECT
  typedef InputComboBox Self;
  typedef ComboBoxEdit Base;

public:
  explicit InputComboBox(QWidget *parent = 0);
};

#endif // INPUTCOMBOBOX_H
