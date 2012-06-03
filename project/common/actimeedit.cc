// actimeedit.cc
// 10/10/2011

#include "project/common/actimeedit.h"
#include "project/common/acss.h"

// - Constructions -

void
AcTimeEdit::init()
{
  setDisplayFormat("h:mm:ss");
  setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  //timeEdit_->setStyleSheet(
  //  SS_BEGIN(QTimeEdit)
  //    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND)
  //    SS_BORDER_WIDTH(4px)
  //    SS_BORDER_RADIUS(3px)
  //  SS_END
  //);
}

// EOF
