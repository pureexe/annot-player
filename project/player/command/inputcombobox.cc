// inputcombobox.cc
// 9/26/2011
//
// 10/11/2011: This class is currently reserved for future specialization.
// For instance, add "\chat" into popup context menu.

#include "inputcombobox.h"
#include <QtGui>

InputComboBox::InputComboBox(QWidget *parent)
  : Base(parent)
{
  QStringList l; {
    l.append(tr("Konichiwa!"));
    l.append("ノシ");
  }
  setDefaultItems(l);
  reset();
}

// EOF
