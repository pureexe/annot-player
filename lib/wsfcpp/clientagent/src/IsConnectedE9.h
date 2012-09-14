

        #ifndef ISCONNECTEDE9_H
        #define ISCONNECTEDE9_H

       /**
        * IsConnectedE9.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.0  Built on : Oct 15, 2009 (12:37:29 IST)
        */

       /**
        *  IsConnectedE9 class
        */

        namespace me_annot_service{
            class IsConnectedE9;
        }
        

        
       #include "IsConnected.h"
          
        #include <axutil_qname.h>
        

        #include <stdio.h>
        #include <OMElement.h>
        #include <ServiceClient.h>
        #include <ADBDefines.h>

namespace me_annot_service
{
        
        

        class IsConnectedE9 {

        private:
             
                axutil_qname_t* qname;
            me_annot_service::IsConnected* property_IsConnected;

                
                bool isValidIsConnected;
            

        /*** Private methods ***/
          

        bool WSF_CALL
        setIsConnectedNil();
            



        /******************************* public functions *********************************/

        public:

        /**
         * Constructor for class IsConnectedE9
         */

        IsConnectedE9();

        /**
         * Destructor IsConnectedE9
         */
        ~IsConnectedE9();


       

        /**
         * Constructor for creating IsConnectedE9
         * @param 
         * @param IsConnected me_annot_service::IsConnected*
         * @return newly created IsConnectedE9 object
         */
        IsConnectedE9(me_annot_service::IsConnected* arg_IsConnected);
        
        
        /********************************** Class get set methods **************************************/
        
        

        /**
         * Getter for isConnected. 
         * @return me_annot_service::IsConnected*
         */
        WSF_EXTERN me_annot_service::IsConnected* WSF_CALL
        getIsConnected();

        /**
         * Setter for isConnected.
         * @param arg_IsConnected me_annot_service::IsConnected*
         * @return true on success, false otherwise
         */
        WSF_EXTERN bool WSF_CALL
        setIsConnected(me_annot_service::IsConnected*  arg_IsConnected);

        /**
         * Re setter for isConnected
         * @return true on success, false
         */
        WSF_EXTERN bool WSF_CALL
        resetIsConnected();
        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether isConnected is Nill
         * @return true if the element is Nil, false otherwise
         */
        bool WSF_CALL
        isIsConnectedNil();


        

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
         * @param IsConnectedE9_om_node node to serialize from
         * @param IsConnectedE9_om_element parent element to serialize from
         * @param tag_closed Whether the parent tag is closed or not
         * @param namespaces hash of namespace uris to prefixes
         * @param next_ns_index an int which contains the next namespace index
         * @return axiom_node_t on success,NULL otherwise.
         */
        axiom_node_t* WSF_CALL
        serialize(axiom_node_t* IsConnectedE9_om_node, axiom_element_t *IsConnectedE9_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the IsConnectedE9 is a particle class (E.g. group, inner sequence)
         * @return true if this is a particle class, false otherwise.
         */
        bool WSF_CALL
        isParticle();



        /******************************* get the value by the property number  *********************************/
        /************NOTE: This method is introduced to resolve a problem in unwrapping mode *******************/

      
        

        /**
         * Getter for isConnected by property number (1)
         * @return me_annot_service::IsConnected
         */

        me_annot_service::IsConnected* WSF_CALL
        getProperty1();

    

};

}        
 #endif /* ISCONNECTEDE9_H */
    

