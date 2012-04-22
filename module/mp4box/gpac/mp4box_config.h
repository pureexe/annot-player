#ifndef MP4BOX_CONFIG_H
#define MP4BOX_CONFIG_H

// mp4box_config.h
// 1/16/2011

#include <QtGlobal>

#ifdef _MSC_VER
#  pragma warning(disable:4996) // C4996: unsafe function or variable used such as stricmp
#  pragma warning(disable:4100) // C4100: unreferenced formal parameter
#endif // _MSC_VER

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wunused-parameter" // unused parameter
#  pragma GCC diagnostic ignored "-Wpointer-sign"     // pointer with diff signess
#endif // __GNUC__

//#ifdef MP4BOX_SHARED
//#  ifdef WIN32
//#    define MP4BOXAPI __declspec(dllexport)
//#  elif defined(QT_VISIBILITY_AVAILABLE)
//#    define MP4BOXAPI __attribute__((visibility("default")))
//#  else
//#    define MP4BOXAPI Q_DECL_EXPORT
//#  endif
//#elif defined(MP4BOX_STATIC)
//#  define MP4BOXAPI
//#else
//#  define MP4BOXAPI Q_DECL_IMPORT
//#endif // MP4BOX_

#endif // MP4BOX_CONFIG_H
