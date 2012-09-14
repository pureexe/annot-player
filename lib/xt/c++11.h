#ifndef CPP11_H
#define CPP11_H

// c++11.h
// 10/16/2011

// See: http://stackoverflow.com/questions/7419472/qtcreator-how-do-i-register-override-and-final-as-a-keyword
#ifdef __clang__
# if __clang__*100 + __clang_minor < 40200
#  define override
//#  define final
# endif // __clang__
#elif defined(__GNUC__)
# if __GNUC__*100 + __GNUC_MINOR__ < 40700
#  define override
//#  define final
# endif // __GNUC__
#elif defined(_MSC_VER)
#  if _MSC_VER < 1600
#   define override
//#   define final
#  endif // _MSC_VER
#else
# define override
//# define final
#endif

#endif // CPP11_H
