#ifndef XDEBUG_H
#define XDEBUG_H

// xdebug.h
// 10/16/2011

#if defined(XDEBUG) && !defined(XT_NO_DEBUG)
# include <iostream>
# define XD(_msg)   std::cerr << XDEBUG << ":" << __FUNCTION__ << ": " \
                                  << _msg << std::endl

#else
# define XD(_dummy) (void)0

  //#ifdef _MSC_VER
  //# pragma warning (disable:4390)     // C4390: empty controlled statement found: is this the intent?
  //#endif // _MSC_VER

  //#ifdef __GNUC__
  //# pragma GCC diagnostic ignored "-Wempty-body" // empty body in an if or else statement
  //#endif // __GNUC__

#endif // XDEBUG

#endif // XDEBUG_H
