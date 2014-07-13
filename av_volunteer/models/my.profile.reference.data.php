<?php

/**
 * Data model class for My Profile -> Contact form
 * @author  andyw@circle
 * @package anvol
 */
class My_Profile_Reference_Data extends Volunteer_Abstract_Data_Model {

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
        'reference1_name'    => 'custom_55',
        'reference1_address' => 'custom_79',
        'reference1_phone'   => 'custom_88',
        'reference2_name'    => 'custom_58',
        'reference2_address' => 'custom_80',
        'reference2_phone'   => 'custom_89',
    );

    /**
     * Address details
     * @var    array
     * @access public
     */
    public $address = array();

    /**
     * Constructor
     */
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

        # check is of correct subtype
        try {
            $contact_sub_type = civicrm_api3('contact', 'getvalue', array(
                'id'     => $this->contact_id,
                'return' => 'contact_sub_type'
            ));
        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting contact sub type for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }       

        if (!in_array('Volunteer', $contact_sub_type)) {
            drupal_set_message(t("Unable to load charity data: Contact is not of type 'Volunteer'"), 'error');
            return $this->error("Error loading charity information for contact id: @contact_id - contact not of type 'Volunteer'", array(
                '@contact_id' => $this->contact_id
            ));
        }       

        # get contact-related custom data
        foreach ($this->custom_data as $custom_field)
            $customData['return.' . $custom_field] = 1; 

        try {
            $this->contact = civicrm_api3('contact', 'getsingle', array('id' => $this->contact_id) + $customData);
        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting contact data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->contact);
        $this->log('references', $this->contact);

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

        return (bool)$success;

    }

}