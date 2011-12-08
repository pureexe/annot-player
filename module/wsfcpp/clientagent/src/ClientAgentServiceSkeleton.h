

    /**
     * ClientAgentServiceSkeleton.h
     *
     * This file was auto-generated from WSDL for "ClientAgentService|http://service.annot.me/" service
     * by the WSO2 WSF/CPP Version: 1.0
     * ClientAgentServiceSkeleton WSO2 WSF/CPP Skeleton for the Service Header File
     */
#ifndef CLIENTAGENTSERVICESKELETON_H
#define CLIENTAGENTSERVICESKELETON_H

    #include <OMElement.h>
    #include <MessageContext.h>
   
     #include <IsAuthorizedE1.h>
    
     #include <IsAuthorizedResponseE5.h>
    
     #include <DeauthorizeE2.h>
    
     #include <DeauthorizeResponseE6.h>
    
     #include <AuthorizeE4.h>
    
     #include <AuthorizeResponseE3.h>
    
     #include <ChatE0.h>
    
     #include <ChatResponseE8.h>
    
     #include <IsAuthorizedE1.h>
    
     #include <IsConnectedResponseE7.h>
    
namespace me_annot_service{
    

   /** we have to reserve some error codes for adb and for custom messages */
    #define CLIENTAGENTSERVICESKELETON_ERROR_CODES_START (AXIS2_ERROR_LAST + 2500)

    typedef enum
    {
        CLIENTAGENTSERVICESKELETON_ERROR_NONE = CLIENTAGENTSERVICESKELETON_ERROR_CODES_START,

        CLIENTAGENTSERVICESKELETON_ERROR_LAST
    } ClientAgentServiceSkeleton_error_codes;

    


class ClientAgentServiceSkeleton
{
        public:
            ClientAgentServiceSkeleton(){}


     




		 


        /**
         * Auto generated method declaration
         * for "isAuthorized|http://service.annot.me/" operation.
         * 
         * @param _isAuthorized of the me_annot_service::IsAuthorizedE1
         *
         * @return me_annot_service::IsAuthorizedResponseE5*
         */
        

         virtual 
        me_annot_service::IsAuthorizedResponseE5* isAuthorized(wso2wsf::MessageContext *outCtx ,me_annot_service::IsAuthorizedE1* _isAuthorized);


     




		 


        /**
         * Auto generated method declaration
         * for "deauthorize|http://service.annot.me/" operation.
         * 
         * @param _deauthorize of the me_annot_service::DeauthorizeE2
         *
         * @return me_annot_service::DeauthorizeResponseE6*
         */
        

         virtual 
        me_annot_service::DeauthorizeResponseE6* deauthorize(wso2wsf::MessageContext *outCtx ,me_annot_service::DeauthorizeE2* _deauthorize);


     




		 


        /**
         * Auto generated method declaration
         * for "authorize|http://service.annot.me/" operation.
         * 
         * @param _authorize of the me_annot_service::AuthorizeE4
         *
         * @return me_annot_service::AuthorizeResponseE3*
         */
        

         virtual 
        me_annot_service::AuthorizeResponseE3* authorize(wso2wsf::MessageContext *outCtx ,me_annot_service::AuthorizeE4* _authorize);


     




		 


        /**
         * Auto generated method declaration
         * for "chat|http://service.annot.me/" operation.
         * 
         * @param _chat of the me_annot_service::ChatE0
         *
         * @return me_annot_service::ChatResponseE8*
         */
        

         virtual 
        me_annot_service::ChatResponseE8* chat(wso2wsf::MessageContext *outCtx ,me_annot_service::ChatE0* _chat);


     




		 


        /**
         * Auto generated method declaration
         * for "isConnected|http://service.annot.me/" operation.
         * 
         * @param _isAuthorized0 of the me_annot_service::IsAuthorizedE1
         *
         * @return me_annot_service::IsConnectedResponseE7*
         */
        

         virtual 
        me_annot_service::IsConnectedResponseE7* isConnected(wso2wsf::MessageContext *outCtx ,me_annot_service::IsAuthorizedE1* _isAuthorized0);


     



};


}



        
#endif // CLIENTAGENTSERVICESKELETON_H
    

