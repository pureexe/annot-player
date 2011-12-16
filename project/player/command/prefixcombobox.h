#ifndef PREFIXCOMBOBOX_H
#define PREFIXCOMBOBOX_H

// prefixcombobox.h
// 9/26/2011

#include "comboboxedit.h"
#include <QVector>

class PrefixComboBox : public ComboBoxEdit
{
  Q_OBJECT
  typedef PrefixComboBox Self;
  typedef ComboBoxEdit Base;

public:
  explicit PrefixComboBox(QWidget *parent = 0);
};

#endif // PREFIXCOMBOBOX_H
