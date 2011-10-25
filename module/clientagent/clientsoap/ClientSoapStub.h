/* ClientSoapStub.h
   Generated by gSOAP 2.8.3 from _clientsoap.h

Copyright(C) 2000-2011, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
1) GPL or 2) Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef ClientSoapStub_H
#define ClientSoapStub_H
#include <vector>
#define SOAP_NAMESPACE_OF_ns1	""
#define SOAP_NAMESPACE_OF_tns	"http://service.annot.me/"
#ifndef WITH_NOGLOBAL
#define WITH_NOGLOBAL
#endif
#include "stdsoap2.h"

namespace ClientSoap {

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_ClientSoap_tns__isAuthorized
#define SOAP_TYPE_ClientSoap_tns__isAuthorized (8)
/* tns:isAuthorized */
class SOAP_CMAC tns__isAuthorized
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 8; } /* = unique id SOAP_TYPE_ClientSoap_tns__isAuthorized */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__isAuthorized() { tns__isAuthorized::soap_default(NULL); }
	virtual ~tns__isAuthorized() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__isAuthorizedResponse
#define SOAP_TYPE_ClientSoap_tns__isAuthorizedResponse (9)
/* tns:isAuthorizedResponse */
class SOAP_CMAC tns__isAuthorizedResponse
{
public:
	bool return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 9; } /* = unique id SOAP_TYPE_ClientSoap_tns__isAuthorizedResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__isAuthorizedResponse() { tns__isAuthorizedResponse::soap_default(NULL); }
	virtual ~tns__isAuthorizedResponse() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__isConnected
#define SOAP_TYPE_ClientSoap_tns__isConnected (10)
/* tns:isConnected */
class SOAP_CMAC tns__isConnected
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 10; } /* = unique id SOAP_TYPE_ClientSoap_tns__isConnected */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__isConnected() { tns__isConnected::soap_default(NULL); }
	virtual ~tns__isConnected() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__isConnectedResponse
#define SOAP_TYPE_ClientSoap_tns__isConnectedResponse (11)
/* tns:isConnectedResponse */
class SOAP_CMAC tns__isConnectedResponse
{
public:
	bool return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 11; } /* = unique id SOAP_TYPE_ClientSoap_tns__isConnectedResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__isConnectedResponse() { tns__isConnectedResponse::soap_default(NULL); }
	virtual ~tns__isConnectedResponse() { }
};
#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_ClientSoap_tns__chat
#define SOAP_TYPE_ClientSoap_tns__chat (12)
/* tns:chat */
class SOAP_CMAC tns__chat
{
public:
	std::string *arg0;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 12; } /* = unique id SOAP_TYPE_ClientSoap_tns__chat */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__chat() { tns__chat::soap_default(NULL); }
	virtual ~tns__chat() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__chatResponse
#define SOAP_TYPE_ClientSoap_tns__chatResponse (13)
/* tns:chatResponse */
class SOAP_CMAC tns__chatResponse
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 13; } /* = unique id SOAP_TYPE_ClientSoap_tns__chatResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__chatResponse() { tns__chatResponse::soap_default(NULL); }
	virtual ~tns__chatResponse() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__authorize
#define SOAP_TYPE_ClientSoap_tns__authorize (14)
/* tns:authorize */
class SOAP_CMAC tns__authorize
{
public:
	LONG64 arg0;	/* required element of type xsd:long */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 14; } /* = unique id SOAP_TYPE_ClientSoap_tns__authorize */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__authorize() { tns__authorize::soap_default(NULL); }
	virtual ~tns__authorize() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__authorizeResponse
#define SOAP_TYPE_ClientSoap_tns__authorizeResponse (15)
/* tns:authorizeResponse */
class SOAP_CMAC tns__authorizeResponse
{
public:
	bool return_;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 15; } /* = unique id SOAP_TYPE_ClientSoap_tns__authorizeResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__authorizeResponse() { tns__authorizeResponse::soap_default(NULL); }
	virtual ~tns__authorizeResponse() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__deauthorize
#define SOAP_TYPE_ClientSoap_tns__deauthorize (16)
/* tns:deauthorize */
class SOAP_CMAC tns__deauthorize
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 16; } /* = unique id SOAP_TYPE_ClientSoap_tns__deauthorize */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__deauthorize() { tns__deauthorize::soap_default(NULL); }
	virtual ~tns__deauthorize() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap_tns__deauthorizeResponse
#define SOAP_TYPE_ClientSoap_tns__deauthorizeResponse (17)
/* tns:deauthorizeResponse */
class SOAP_CMAC tns__deauthorizeResponse
{
public:
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 17; } /* = unique id SOAP_TYPE_ClientSoap_tns__deauthorizeResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         tns__deauthorizeResponse() { tns__deauthorizeResponse::soap_default(NULL); }
	virtual ~tns__deauthorizeResponse() { }
};
#endif

#ifndef SOAP_TYPE_ClientSoap___tns__authorize
#define SOAP_TYPE_ClientSoap___tns__authorize (26)
/* Operation wrapper: */
struct __tns__authorize
{
public:
	tns__authorize *tns__authorize_;	/* optional element of type tns:authorize */
};
#endif

#ifndef SOAP_TYPE_ClientSoap___tns__deauthorize
#define SOAP_TYPE_ClientSoap___tns__deauthorize (30)
/* Operation wrapper: */
struct __tns__deauthorize
{
public:
	tns__deauthorize *tns__deauthorize_;	/* optional element of type tns:deauthorize */
};
#endif

#ifndef SOAP_TYPE_ClientSoap___tns__isAuthorized
#define SOAP_TYPE_ClientSoap___tns__isAuthorized (34)
/* Operation wrapper: */
struct __tns__isAuthorized
{
public:
	tns__isAuthorized *tns__isAuthorized_;	/* optional element of type tns:isAuthorized */
};
#endif

#ifndef SOAP_TYPE_ClientSoap___tns__isConnected
#define SOAP_TYPE_ClientSoap___tns__isConnected (38)
/* Operation wrapper: */
struct __tns__isConnected
{
public:
	tns__isConnected *tns__isConnected_;	/* optional element of type tns:isConnected */
};
#endif

#ifndef SOAP_TYPE_ClientSoap___tns__chat
#define SOAP_TYPE_ClientSoap___tns__chat (42)
/* Operation wrapper: */
struct __tns__chat
{
public:
	tns__chat *tns__chat_;	/* optional element of type tns:chat */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_ClientSoap_SOAP_ENV__Header
#define SOAP_TYPE_ClientSoap_SOAP_ENV__Header (43)
/* SOAP Header: */
struct SOAP_ENV__Header
{
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_ClientSoap_SOAP_ENV__Code
#define SOAP_TYPE_ClientSoap_SOAP_ENV__Code (44)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_ClientSoap_SOAP_ENV__Detail
#define SOAP_TYPE_ClientSoap_SOAP_ENV__Detail (46)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_ClientSoap_SOAP_ENV__Reason
#define SOAP_TYPE_ClientSoap_SOAP_ENV__Reason (49)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_ClientSoap_SOAP_ENV__Fault
#define SOAP_TYPE_ClientSoap_SOAP_ENV__Fault (50)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE_ClientSoap__QName
#define SOAP_TYPE_ClientSoap__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE_ClientSoap__XML
#define SOAP_TYPE_ClientSoap__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


} // namespace ClientSoap


#endif

/* End of ClientSoapStub.h */
