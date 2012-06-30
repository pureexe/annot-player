#ifndef ANNOTATIONGRAPHICSSTYLE_H
#define ANNOTATIONGRAPHICSSTYLE_H

// annotationgraphicsstyle.h
// 7/28/2011
#include <QtGlobal>

// - Contants -

//QColor(r, g, b, a), sample color RGB palatte: http://www.tayloredmktg.com/rgb/
#define ANNOTATION_COLOR_DEFAULT    QColor(255, 250, 250, 255) // Disable alpha for better performance.

#define ANNOTATION_SIZE_DEFAULT         20
#define ANNOTATION_SIZE_MARGIN          2

#define ANNOTATION_SIZE_TINY            "10"
#define ANNOTATION_SIZE_SMALL           "18"
#define ANNOTATION_SIZE_NORMAL          "25"
#define ANNOTATION_SIZE_LARGE           "36"
#define ANNOTATION_SIZE_HUGE            "60"

#define ANNOTATION_STAY_TIME            2000
#define ANNOTATION_STAY_TIME_SUBTITLE   3000
#define ANNOTATION_STAY_TIME_MIN        1000
#define ANNOTATION_STAY_TIME_MAX        10000
#define ANNOTATION_FLY_TIME             7000 // the larger the slower
#define ANNOTATION_FLY_TIME_MIN         1500
#define ANNOTATION_FLY_TIME_MAX         15000

#endif // ANNOTATIONGRAPHICSSTYLE_H
