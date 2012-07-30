// module/clientagent/clientsoap.cc
// 10/6/2011

#include "clientsoap.h"

//#include "clientsoap/ClientAgentServiceSoapBinding.nsmap"
#include "clientsoap/ClientSoapC.cpp"
#include "clientsoap/ClientSoapClientAgentServiceSoapBindingService.cpp"

using namespace ClientSoap;
#define SOAP_DEFMAIN    __dummy_methods__
#include "clientsoap/ClientSoapTester.cpp"

// EOF

// - Dummy methods -

/*
int
ClientSoap::ClientSoapService::
authorize(ns1__authorize *, ns1__authorizeResponse *)
{ return SOAP_OK; }

int
ClientSoap::ClientSoapService::
deauthorize(ns1__deauthorize *, ns1__deauthorizeResponse *)
{ return SOAP_OK; }

int
ClientSoap::ClientSoapService::
isAuthorized(ns1__isAuthorized *, ns1__isAuthorizedResponse *)
{ return SOAP_OK; }

int
ClientSoap::ClientSoapService::
isConnected(ns1__isAuthorized *, ns1__isConnectedResponse *)
{ return SOAP_OK; }

int
ClientSoap::ClientSoapService::
chat(ns1__chat *, ns1__chatResponse *)
{ return SOAP_OK; }
*/
