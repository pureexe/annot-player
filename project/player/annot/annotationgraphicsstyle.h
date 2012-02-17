#ifndef ANNOTATIONGRAPHICSSTYLE_H
#define ANNOTATIONGRAPHICSSTYLE_H

// annotationgraphicsstyle.h
// 7/28/2011
#include <QtGlobal>

// - Contants -

//QColor(r, g, b, a), sample color RGB palatte: http://www.tayloredmktg.com/rgb/
#define ANNOTATION_COLOR_DEFAULT    QColor(255, 250, 250, 255) // Disable alpha for better performance.

//#ifdef Q_WS_MAC
  #define ANNOTATION_SIZE_DEFAULT       20
  #define ANNOTATION_SIZE_MARGIN        2

  #define ANNOTATION_SIZE_TINY          "10px"
  #define ANNOTATION_SIZE_SMALL         "18px"
  #define ANNOTATION_SIZE_NORMAL        "25px"
  #define ANNOTATION_SIZE_LARGE         "36px"
  #define ANNOTATION_SIZE_HUGE          "60px"
//#else
//  #define ANNOTATION_SIZE_DEFAULT       15 // arround 20px
//  #define ANNOTATION_SIZE_MARGIN        2
//
//  #define ANNOTATION_SIZE_TINY          "10px"
//  #define ANNOTATION_SIZE_SMALL         "15px"
//  #define ANNOTATION_SIZE_NORMAL        "20px"
//  #define ANNOTATION_SIZE_LARGE         "40px"
//  #define ANNOTATION_SIZE_HUGE          "80px"
//#endif // Q_WS_MAC

#define ANNOTATION_FONT_ENGLISH         "Arial"
#define ANNOTATION_FONT_CHINESE         "SimSun"
#define ANNOTATION_FONT_JAPANESE        "MS Gothic"
#define ANNOTATION_FONT_DEFAULT         ANNOTATION_FONT_JAPANESE

#define ANNOTATION_STAY_TIME            2000
#define ANNOTATION_STAY_TIME_SUBTITLE   3000
#define ANNOTATION_STAY_TIME_MIN        1000
#define ANNOTATION_STAY_TIME_MAX        10000
#define ANNOTATION_FLY_TIME             6000 // the larger the slower
#define ANNOTATION_FLY_TIME_MIN         1000
#define ANNOTATION_FLY_TIME_MAX         10000

#define ANNOTATION_OPACITY          0.8 // [0, 1]

#endif // ANNOTATIONGRAPHICSSTYLE_H
