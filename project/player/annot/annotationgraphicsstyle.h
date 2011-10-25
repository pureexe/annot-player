#ifndef ANNOTATIONGRAPHICSSTYLE_H
#define ANNOTATIONGRAPHICSSTYLE_H

// annotationgraphicsstyle.h
// 7/28/2011

// - Contants -

//QColor(r, g, b, a), sample color RGB palatte: http://www.tayloredmktg.com/rgb/
#define ANNOTATION_COLOR_DEFAULT    QColor(255, 250, 250, 255) // Disable alpha for better performance.

#define ANNOTATION_SIZE_DEFAULT     15 // arround 20px
#define ANNOTATION_SIZE_MARGIN      2

#define ANNOTATION_SIZE_TINY        "10px"
#define ANNOTATION_SIZE_SMALL       "15px"
#define ANNOTATION_SIZE_NORMAL      "20px"
#define ANNOTATION_SIZE_LARGE       "40px"
#define ANNOTATION_SIZE_HUGE        "80px"


#define ANNOTATION_FONT_ENGLISH     "Arial"
#define ANNOTATION_FONT_CHINESE     "SimSun"
#define ANNOTATION_FONT_DEFAULT     ANNOTATION_FONT_CHINESE

#define ANNOTATION_STAY_TIME        4500
#define ANNOTATION_FLY_SPEED        150     // pixal per secs, not msecs!

#define ANNOTATION_OPACITY          0.9     // [0, 1]

#endif // ANNOTATIONGRAPHICSSTYLE_H
