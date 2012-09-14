

          #ifndef CLIENTAGENTSERVICE_H
          #define CLIENTAGENTSERVICE_H

        /**
         * ClientAgentService.h
         *
         * This file was auto-generated from WSDL for "ClientAgentService|http://service.annot.me/" service
         * by the Apache Axis2 version: 1.0  Built on : Oct 15, 2009 (12:36:59 IST)
         *  ClientAgentService
         */

#include <ServiceSkeleton.h>
#include <stdio.h>
#include <axis2_svc.h>

using namespace wso2wsf;


using namespace me_annot_service;



#define WSF_SERVICE_SKEL_INIT(class_name) \
ClientAgentServiceSkeleton* wsfGetClientAgentServiceSkeleton(){ return new class_name(); }

ClientAgentServiceSkeleton* wsfGetClientAgentServiceSkeleton(); 



        class ClientAgentService : public ServiceSkeleton
        {
            private:
                ClientAgentServiceSkeleton *skel;

            public:

               union {
                     
               } fault;


              WSF_EXTERN WSF_CALL ClientAgentService();

              OMElement* WSF_CALL invoke(OMElement *message, MessageContext *msgCtx);

              OMElement* WSF_CALL onFault(OMElement *message);

              void WSF_CALL init();

              ~ClientAgentService(); 
      };



#endif    //     CLIENTAGENTSERVICE_H

    

