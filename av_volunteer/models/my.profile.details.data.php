<?php

/**
 * Data model class for My Profile -> Contact form
 * @author  andyw@circle
 * @package anvol
 */
class My_Profile_Details_Data extends Volunteer_Abstract_Data_Model {

    /**
     * Contact details
     * @var    array
     * @access public
     */
    public $contact = array();

    /**
     * Charity details
     * @var    array
     * @access public
     */
    public $charity = array();

    /**
     * Custom data friendly key => civi key map
     * @var    array
     * @access public
     */
    public $custom_data = array(
        'charity_no'     => 'custom_69',
        'charity_desc'   => 'custom_21',
        'where_you_hear' => 'custom_71' 
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
        
        $custom = &$this->custom;

        # get contact data for the contact
        try {

            $this->contact = civicrm_api3('contact', 'getsingle', array(
                'id' => $this->contact_id
            ) + array(
                'return.' . $custom['charity_no']     => 1,
                'return.' . $custom['charity_desc']   => 1,
                'return.' . $custom['umbrella']       => 1,
                'return.' . $custom['where_you_hear'] => 1,
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting contact data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->contact);
        $this->log('contact', $this->contact);

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

        # convert friendly keys to internal custom data keys
        $this->createCiviKeys($this->contact);

        # save contact details
        try {
            civicrm_api3('contact', 'create', $this->contact);
        } catch (CiviCRM_API3_Exception $e) {
            $success = $this->error("Error saving contact data for contact id @contact_id: @excuse @debug", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage(),
                '@debug'      => $debug($this->contact)
            ));
        }

        return (bool)$success;

    }

}