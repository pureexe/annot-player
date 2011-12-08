

        #ifndef ISAUTHORIZEDE1_H
        #define ISAUTHORIZEDE1_H

       /**
        * IsAuthorizedE1.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.0  Built on : Oct 15, 2009 (12:37:29 IST)
        */

       /**
        *  IsAuthorizedE1 class
        */

        namespace me_annot_service{
            class IsAuthorizedE1;
        }
        

        
       #include "IsAuthorized.h"
          
        #include <axutil_qname.h>
        

        #include <stdio.h>
        #include <OMElement.h>
        #include <ServiceClient.h>
        #include <ADBDefines.h>

namespace me_annot_service
{
        
        

        class IsAuthorizedE1 {

        private:
             
                axutil_qname_t* qname;
            me_annot_service::IsAuthorized* property_IsAuthorized;

                
                bool isValidIsAuthorized;
            

        /*** Private methods ***/
          

        bool WSF_CALL
        setIsAuthorizedNil();
            



        /******************************* public functions *********************************/

        public:

        /**
         * Constructor for class IsAuthorizedE1
         */

        IsAuthorizedE1();

        /**
         * Destructor IsAuthorizedE1
         */
        ~IsAuthorizedE1();


       

        /**
         * Constructor for creating IsAuthorizedE1
         * @param 
         * @param IsAuthorized me_annot_service::IsAuthorized*
         * @return newly created IsAuthorizedE1 object
         */
        IsAuthorizedE1(me_annot_service::IsAuthorized* arg_IsAuthorized);
        
        
        /********************************** Class get set methods **************************************/
        
        

        /**
         * Getter for isAuthorized. 
         * @return me_annot_service::IsAuthorized*
         */
        WSF_EXTERN me_annot_service::IsAuthorized* WSF_CALL
        getIsAuthorized();

        /**
         * Setter for isAuthorized.
         * @param arg_IsAuthorized me_annot_service::IsAuthorized*
         * @return true on success, false otherwise
         */
        WSF_EXTERN bool WSF_CALL
        setIsAuthorized(me_annot_service::IsAuthorized*  arg_IsAuthorized);

        /**
         * Re setter for isAuthorized
         * @return true on success, false
         */
        WSF_EXTERN bool WSF_CALL
        resetIsAuthorized();
        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether isAuthorized is Nill
         * @return true if the element is Nil, false otherwise
         */
        bool WSF_CALL
        isIsAuthorizedNil();


        

        /**************************** Serialize and De serialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize the ADB object to an XML
         * @param dp_parent double pointer to the parent node to be deserialized
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return true on success, false otherwise
         */
        bool WSF_CALL
        deserialize(axiom_node_t** omNode, bool *isEarlyNodeValid, bool dontCareMinoccurs);
                         
            

       /**
         * Declare namespace in the most parent node 
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
        void WSF_CALL
        declareParentNamespaces(axiom_element_t *parent_element, axutil_hash_t *namespaces, int *next_ns_index);


        

        /**
         * Serialize the ADB object to an xml
         * @param IsAuthorizedE1_om_node node to serialize from
         * @param IsAuthorizedE1_om_element parent element to serialize from
         * @param tag_closed Whether the parent tag is closed or not
         * @param namespaces hash of namespace uris to prefixes
         * @param next_ns_index an int which contains the next namespace index
         * @return axiom_node_t on success,NULL otherwise.
         */
        axiom_node_t* WSF_CALL
        serialize(axiom_node_t* IsAuthorizedE1_om_node, axiom_element_t *IsAuthorizedE1_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the IsAuthorizedE1 is a particle class (E.g. group, inner sequence)
         * @return true if this is a particle class, false otherwise.
         */
        bool WSF_CALL
        isParticle();



        /******************************* get the value by the property number  *********************************/
        /************NOTE: This method is introduced to resolve a problem in unwrapping mode *******************/

      
        

        /**
         * Getter for isAuthorized by property number (1)
         * @return me_annot_service::IsAuthorized
         */

        me_annot_service::IsAuthorized* WSF_CALL
        getProperty1();

    

};

}        
 #endif /* ISAUTHORIZEDE1_H */
    

