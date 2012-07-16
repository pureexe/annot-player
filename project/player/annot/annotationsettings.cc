// annotationsettings.cc
// 8/22/2011

#include "annotationsettings.h"
#include "module/annotcloud/annothtml.h"

namespace { struct parser_init_ {
  // Potential run time error if this class is linked before PARSER get initialized.
  parser_init_()
  {
    auto p = AnnotCloud::AnnotationHtmlParser::globalInstance();

    p->setTinySize(ANNOTATION_SIZE_TINY "px");
    p->setSmallSize(ANNOTATION_SIZE_SMALL "px");
    p->setNormalSize(ANNOTATION_SIZE_NORMAL "px");
    p->setLargeSize(ANNOTATION_SIZE_LARGE "px");
    p->setHugeSize(ANNOTATION_SIZE_HUGE "px");
  }
} parser_init_; }

// EOF
