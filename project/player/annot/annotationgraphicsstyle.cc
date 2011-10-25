// annotationgraphicsstyle.cc
// 8/22/2011

#include "annotationgraphicsstyle.h"
#include "core/annotationparser.h"

#define PARSER  Core::AnnotationParser::globalInstance()

namespace { // anonymous, static initializers

  struct parser_config {

    parser_config()
    {
      Q_ASSERT(PARSER);

      PARSER->setTinySize(ANNOTATION_SIZE_TINY);
      PARSER->setSmallSize(ANNOTATION_SIZE_SMALL);
      PARSER->setNormalSize(ANNOTATION_SIZE_NORMAL);
      PARSER->setLargeSize(ANNOTATION_SIZE_LARGE);
      PARSER->setHugeSize(ANNOTATION_SIZE_HUGE);
    }

  } parser_config;

} // anonymous namespace

// EOF
