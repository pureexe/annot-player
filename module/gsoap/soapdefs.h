#ifndef SOAPDEFS_H
#define SOAPDEFS_H
// soapdefs.h
// 11/4/2011
// Don't forget to regenerate envC.cpp after modifying this file.

#ifdef _MSC_VER
  #pragma warning (disable:4100)  // C4100: unreferenced formal parameter
  #pragma warning (disable:4244)  // C4244: conversion from 'std::streamsize' to 'size_t', possible loss of data
#endif // _MSC_VER
#ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wunused-parameter" // unused function parameters
  #pragma GCC diagnostic ignored "-Wsequence-point" // soap::bufidx
#endif // __GNUC__

// Increase buffer length
// http://www.cs.fsu.edu/~engelen/soapdoc2.html#tth_sEc19.18
// Increase the buffer size SOAP_BUFLEN by changing the SOAP_BUFLEN macro in stdsoap2.h. Use buffer size 2^18=262144 for example.
//#define SOAP_BUFLEN     262144  // 2^18

#endif // SOAPDEFS_H
