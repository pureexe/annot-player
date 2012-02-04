// prefixcombobox.cc
// 9/26/2011
//
// 10/11/2011: This class is currently reserved for future specialization.
// For instance, add "\chat" into popup context menu.

#include "prefixcombobox.h"
#include "module/annotcloud/cmd.h"
#include <QtGui>

PrefixComboBox::PrefixComboBox(QWidget *parent)
  : Base(parent)
{
  QStringList l; {
    l.append(CORE_CMD_VIEW_TOP); l.append(CORE_CMD_VIEW_BOTTOM);
    l.append(CORE_CMD_LATEX_BIG);
    l.append(CORE_CMD_COLOR_RED); l.append(CORE_CMD_COLOR_BLUE); l.append(CORE_CMD_COLOR_YELLOW);
    l.append(CORE_CMD_LATEX_UNDERLINE); l.append(CORE_CMD_STYLE_STRIKE);
    l.append(CORE_CMD_SUB);
    l.append(CORE_CMD_VERBATIM);
  }
  setDefaultItems(l);
  reset();
}

// EOF
