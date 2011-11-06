// module/serveragent/serversoap.cc
// 9/10/2011

#include "serversoap.h"

#ifdef _MSC_VER
  #pragma warning (disable:4100)  // C4100: unreferenced formal parameter
#endif // _MSC_VER
#ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wunused-parameter" // unused function parameters
#endif // __GNUC__

//#include "serversoap/ServerAgentServiceSoapBinding.nsmap"
#include "serversoap/ServerSoapC.cpp"
#include "serversoap/ServerSoapServerAgentServiceSoapBindingProxy.cpp"

// EOF
