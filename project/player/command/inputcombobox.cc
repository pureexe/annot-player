// inputcombobox.cc
// 9/26/2011
//
// 10/11/2011: This class is currently reserved for future specialization.
// For instance, add "\chat" into popup context menu.

#include "inputcombobox.h"
#include "module/annotcloud/annottag.h"
#include <QtGui>

InputComboBox::InputComboBox(QWidget *parent)
  : Base(parent)
{
  lineEdit()->setPlaceholderText(tr("Comment"));

  setContextMenuFlags(contextMenuFlags() | PasteAndGoAction | EditAction);

#define ADD(_cmd)       defaultItems.append(_cmd)
  ADD("ノシ");
  ADD("\\em 朝\\red{ミルキ}");
  ADD("\\cyan だが{\\big\\red 男}だ");
  ADD(CORE_CMD_COLOR_BACKGROUND "[yellow]" "{背景色}");
  ADD(CORE_CMD_HTML_FONT "[face:gothic;color:red;size:20]" "{HTMLフォント}");
  ADD(CORE_CMD_STYLE "[font-family:gothic;text-decoration:underline]" " HTML CSS3");
  //ADD("\\fly 飛~~~~~~~~~~~~~~~~");
  //ADD("\\repeat[5] クンカ\\cyan{クンカ}");
  ADD("line1\\n\\orange{line2}\\n\\pink{line3}");
  //ADD("\\verbatim \\(>_<)/");
#undef ADD
  setMaxVisibleItems(15);
  reset();
}

// EOF
