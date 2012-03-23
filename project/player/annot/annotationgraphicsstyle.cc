// annotationgraphicsstyle.cc
// 8/22/2011

#include "annotationgraphicsstyle.h"
#include "module/annotcloud/annothtml.h"
#include <boost/typeof/typeof.hpp>

namespace { struct parser_config_ {

    // Potential run time error if this class is linked before PARSER get initialized.
  parser_config_()
  {
    BOOST_AUTO(p, AnnotCloud::AnnotationHtmlParser::globalInstance());

    p->setTinySize(ANNOTATION_SIZE_TINY);
    p->setSmallSize(ANNOTATION_SIZE_SMALL);
    p->setNormalSize(ANNOTATION_SIZE_NORMAL);
    p->setLargeSize(ANNOTATION_SIZE_LARGE);
    p->setHugeSize(ANNOTATION_SIZE_HUGE);
  }

} parser_config_; }

// EOF
