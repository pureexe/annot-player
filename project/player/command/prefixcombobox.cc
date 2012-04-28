// prefixcombobox.cc
// 9/26/2011
//
// 10/11/2011: This class is currently reserved for future specialization.
// For instance, add "\chat" into popup context menu.

#include "prefixcombobox.h"
#include "module/annotcloud/annottag.h"
#include <QtGui>

PrefixComboBox::PrefixComboBox(QWidget *parent)
  : Base(parent)
{
  lineEdit()->setPlaceholderText(tr("Prefix"));
#define ADD(_cmd)       defaultItems.append(_cmd)
  ADD(CORE_CMD_VIEW_TOP); ADD(CORE_CMD_VIEW_BOTTOM); ADD(CORE_CMD_VIEW_FLY);
  ADD(CORE_CMD_LATEX_SMALL); ADD(CORE_CMD_LATEX_BIG);
  ADD(CORE_CMD_COLOR_RED); ADD(CORE_CMD_COLOR_BROWN); ADD(CORE_CMD_COLOR_YELLOW);
  ADD(CORE_CMD_STYLE_STRIKE);
  ADD(CORE_CMD_SUB);
  ADD(CORE_CMD_VERBATIM);
  ADD(CORE_CMD_REPEAT "[10]");
  ADD(CORE_CMD_SIZE "[20]");
  ADD(CORE_CMD_COLOR "[ffffff]");
#undef ADD
  setMaxVisibleItems(20);
  reset();
}

// EOF
