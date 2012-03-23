// annotcloud/annothtml.cc
// 8/19/2011

#include "module/annotcloud/annothtml.h"

#define DEFAULT_TINY_SIZE       "10px"
#define DEFAULT_SMALL_SIZE      "18px"
#define DEFAULT_NORMAL_SIZE     "25px"
#define DEFAULT_LARGE_SIZE      "36px"
#define DEFAULT_HUGE_SIZE       "50px"

// - Construction -

AnnotCloud::
AnnotationHtmlParser::AnnotationHtmlParser()
  : tinySize_(DEFAULT_TINY_SIZE),
    smallSize_(DEFAULT_SMALL_SIZE),
    normalSize_(DEFAULT_NORMAL_SIZE),
    largeSize_(DEFAULT_LARGE_SIZE),
    hugeSize_(DEFAULT_HUGE_SIZE)
{ }

// EOF
