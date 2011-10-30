

        #ifndef AUTHORIZE_H
        #define AUTHORIZE_H

       /**
        * Authorize.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.0  Built on : Oct 15, 2009 (12:37:29 IST)
        */

       /**
        *  Authorize class
        */

        namespace me_annot_service{
            class Authorize;
        }
        

        

        #include <stdio.h>
        #include <OMElement.h>
        #include <ServiceClient.h>
        #include <ADBDefines.h>

namespace me_annot_service
{
        
        

        class Authorize {

        private:
             int64_t property_Arg0;

                
                bool isValidArg0;
            

        /*** Private methods ***/
          

        bool WSF_CALL
        setArg0Nil();
            



        /******************************* public functions *********************************/

        public:

        /**
         * Constructor for class Authorize
         */

        Authorize();

        /**
         * Destructor Authorize
         */
        ~Authorize();


       

        /**
         * Constructor for creating Authorize
         * @param 
         * @param Arg0 int64_t
         * @return newly created Authorize object
         */
        Authorize(int64_t arg_Arg0);
        
        
        /********************************** Class get set methods **************************************/
        
        

        /**
         * Getter for arg0. 
         * @return int64_t*
         */
        WSF_EXTERN int64_t WSF_CALL
        getArg0();

        /**
         * Setter for arg0.
         * @param arg_Arg0 int64_t*
         * @return true on success, false otherwise
         */
        WSF_EXTERN bool WSF_CALL
        setArg0(const int64_t  arg_Arg0);

        /**
         * Re setter for arg0
         * @return true on success, false
         */
        WSF_EXTERN bool WSF_CALL
        resetArg0();
        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether arg0 is Nill
         * @return true if the element is Nil, false otherwise
         */
        bool WSF_CALL
        isArg0Nil();


        

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
         * @param Authorize_om_node node to serialize from
         * @param Authorize_om_element parent element to serialize from
         * @param tag_closed Whether the parent tag is closed or not
         * @param namespaces hash of namespace uris to prefixes
         * @param next_ns_index an int which contains the next namespace index
         * @return axiom_node_t on success,NULL otherwise.
         */
        axiom_node_t* WSF_CALL
        serialize(axiom_node_t* Authorize_om_node, axiom_element_t *Authorize_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the Authorize is a particle class (E.g. group, inner sequence)
         * @return true if this is a particle class, false otherwise.
         */
        bool WSF_CALL
        isParticle();



        /******************************* get the value by the property number  *********************************/
        /************NOTE: This method is introduced to resolve a problem in unwrapping mode *******************/

      
        

        /**
         * Getter for arg0 by property number (1)
         * @return int64_t
         */

        int64_t WSF_CALL
        getProperty1();

    

};

}        
 #endif /* AUTHORIZE_H */
    

