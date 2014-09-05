<?php

/**
 * Data model class for My Profile -> Skills form
 * @author  andyw@circle
 * @package anvol
 */
class My_Profile_Skills_Data extends Volunteer_Abstract_Data_Model {

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
        'level_of_qualification' => 'custom_70',
        'skills_strategy'        => 'custom_97',
        'skills_business'        => 'custom_98',
        'skills_solution'        => 'custom_99',
        'skills_technologies'    => 'custom_100',
        'skills_service'         => 'custom_101',
        'cv_file_id'             => 'custom_95'
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

    protected function deleteCVFile() {

        $config = CRM_Core_Config::singleton();
        watchdog('andyw', 'config = <pre>' . print_r($config, true) . '</pre>');

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

        # load file info
        if (isset($this->contact['cv_file_id']) and !empty($this->contact['cv_file_id']))
            $this->contact['cv_file'] = $this->loadFile($this->contact['cv_file_id']);   

        $this->log('skills', $this->contact);

    }

    /**
     * Load civicrm_file object for the specified file id
     * @param  int   $file_id
     * @return array 
     * @access protected
     */
    protected function loadFile($file_id) {
        try {
            $this->contact['cv_file'] = civicrm_api3('file', 'getsingle', array('id' => $this->contact['cv_file_id']));
        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting cv file metadata for @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }
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
        $this->log('saving skills', $this->contact);

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

        # delete cv file if requested
        if ($this->contact['delete_cv_file'])
            $this->deleteFile();
        

        return (bool)$success;

    }

}