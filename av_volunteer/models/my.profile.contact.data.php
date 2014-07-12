<?php

/**
 * Data model class for My Profile -> Contact form
 * @author  andyw@circle
 * @package anvol
 */
class My_Profile_Contact_Data extends Volunteer_Abstract_Data_Model {

    /**
     * Contact details
     * @var    array
     * @access public
     */
    public $contact = array();

    /**
     * Custom data friendly key => civi key map
     * @var    array
     * @access public
     */
    public $custom_data = array(
        'status' => 'custom_81'
    );

    /**
     * Address details
     * @var    array
     * @access public
     */
    public $address = array();


    public function __construct($uid = null) {
        parent::__construct($uid);
    }

    /**
     * Load form data
     * @access public
     */
    public function load() {

        if (!$this->contact_id)
            return $this->error("Attempt to run @method() on an empty contact id in @class", array(
                '@method' => __METHOD__,
                '@class'  => __CLASS__
            ));           
        
        # get contact data for the contact
        try {

            $this->contact = civicrm_api3('contact', 'getsingle', array(
                'id'                => $this->contact_id,
                'return.first_name' => 1,
                'return.last_name'  => 1,
                'return.nick_name'  => 1,
                'return.prefix_id'  => 1
            ) + array(
                'return.' . $this->custom_data['status'] => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting contact data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->contact);

        # get primary address for contact
        try {

            $result = civicrm_api3('address', 'get', array(
                'contact_id' => $this->contact_id,
                'is_primary' => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->warning("Unable to get address data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->address = $result['count'] ? reset($result['values']) : array(); 

        # get phone numbers for contact
        try {

            $result = civicrm_api3('phone', 'get', array(
                'contact_id' => $this->contact_id,
                'sequential' => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->warning("Unable to get phone data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }      

        $this->phone = $result['count'] ? $result['values'] : array();

        $this->phone = array(
            'main'        => array(),
            'alternative' => array(),
            'mobile'      => array()
        );

        if ($result['count']) {

            # main
            foreach ($result['values'] as $number) {
                if ($number['is_primary']) {
                    $this->phone['main'] = $number;
                    break;
                }
            }

            # alternative
            foreach ($result['values'] as $number) {
                if ($number['location_type_id'] == 4) { # 'Other'
                    $this->phone['alternative'] = $number;
                    break;
                }
            }

            # mobile
            foreach ($result['values'] as $number) {
                if ($number['phone_type_id'] == 2) {
                    $this->phone['mobile'] = $number;
                    break;
                }
            }

        }

        # get primary email for contact
        try {

            $result = civicrm_api3('email', 'get', array(
                'contact_id' => $this->contact_id,
                'is_primary' => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->warning("Unable to get email data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->email = $result['count'] ? reset($result['values']) : array();

        /*
        $this->log('contact', $this->contact);
        $this->log('address', $this->address);  
        $this->log('phone',   $this->phone);
        $this->log('email',   $this->email);    
        $this->log('user',    $this->user); 
        */

    }

    /**
     * Save form data
     * @return bool  success
     * @access public
     */
    public function save() {

        # helper for logging objects being saved
        $debug = function($var) {
            return "<br />API params: <pre>" . print_r($var, true) . '</pre>';
        };

        # naively assume success initially
        $success = true;

        # create Civi keys from friendly custom data keys
        $this->createCiviKeys($this->contact);

        $this->log('contact', $this->contact);

        # save contact details
        try {
            civicrm_api3('contact', 'create', $this->contact);
        } catch (CiviCRM_API3_Exception $e) {
            $success = $this->error("Error saving contact data for contact id @contact_id: @excuse !debug", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage(),
                '!debug'      => $debug($this->contact)
            ));
        }

        # save address details
        if (!isset($this->address['contact_id']))
            $this->address['contact_id'] = $this->contact['id'];

        if (!isset($this->address['location_type_id']))
            $this->address['location_type_id'] = 1;

        try {
            civicrm_api3('address', 'create', $this->address);
        } catch (CiviCRM_API3_Exception $e) {
            $success = $this->error("Error saving address data for contact id @contact_id: @excuse !debug", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage(),
                '!debug'      => $debug($this->address)
            ));
        }

        # save phone numbers

        $location_types = array(
            'main'        => 1, # Home
            'alternative' => 4, # Other
            'mobile'      => 1  # Home
        );

        $phone_types = array(
            'main'        => 1, # Phone
            'alternative' => 1, # Phone
            'mobile'      => 2  # Mobile
        );

        foreach ($this->phone as $type => $phone) {
            
            if (isset($phone['phone']) and !empty($phone['phone'])) {

                # if inserting new record, set required keys
                # when updating, these will already be populated

                if (!isset($phone['contact_id']))
                    $phone['contact_id'] = $this->contact['id'];
                if (!isset($phone['location_type_id']))
                    $phone['location_type_id'] = $location_types[$type];
                if (!isset($phone['phone_type_id']))
                    $phone['phone_type_id']    = $phone_types[$type];
        
                # unset 'phone_numeric', will get recreated as necessary
                unset($phone['phone_numeric']);

                try {
                    civicrm_api3('phone', 'create', $phone);
                } catch (CiviCRM_API3_Exception $e) {
                    $success = $this->error("Error saving phone data for contact id @contact_id: @excuse !debug", array(
                        '@contact_id' => $this->contact_id,
                        '@excuse'     => $e->getMessage(),
                        '!debug'      => $debug($phone)
                    ));
                }

            }
        
        }

        # save email details
        if (!isset($this->email['contact_id']))
            $this->email['contact_id'] = $this->contact['id'];

        if (!isset($this->email['is_primary']) or !$this->email['is_primary'])
            $this->email['is_primary'] = 1;

        if (!isset($this->email['location_type_id']))
            $this->email['location_type_id'] = 1;

        try {
            civicrm_api3('email', 'create', $this->email);
        } catch (CiviCRM_API3_Exception $e) {
            $success = $this->error("Error saving email address data for contact id @contact_id: @excuse !debug", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage(),
                '!debug'      => $debug($this->email)
            ));
        }

        return (bool)$success;

    }

}