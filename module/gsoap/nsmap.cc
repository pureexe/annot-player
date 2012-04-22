// nsmap.cc
// 10/6/2011
// This file is supposed to be consistent with all *.nsmap.
#ifndef WITH_NONAMESPACES

#include "stdsoap2.h"

SOAP_NMAC struct Namespace namespaces[] = {
  { "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL },
  { "SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL },
  { "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
  { "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
  { "tns", "http://service.annot.me/", NULL, NULL },
  { NULL, NULL, NULL, NULL }
};

#endif // WITH_NONAMESPACES

// EOF
