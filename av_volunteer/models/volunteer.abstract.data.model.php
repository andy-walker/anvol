<?php

/**
 * Abstract base class for creating Volunteer form data models
 * @author  andyw@circle
 * @package anvol
 * @abstract
 */
abstract class Volunteer_Abstract_Data_Model {

    /**
     * The user currently being operated on
     * @var    int
     * @access public
     */
    public $user;

    /**
     * The contact id of the user currently being operated on
     * @var    int
     * @access public
     */
    public $contact_id;

    /**
     * Constructor
     * @param int $uid  optional uid - if supplied, load data
     */
    public function __construct($uid = null) {
        
        # in case it's not already been initialized ..
        civicrm_initialize();

        # if user id supplied, lookup contact id then call load()
        if ($this->user = user_load($uid) and $this->contact_id = $this->getContactID($uid))
            $this->load();

    }

    /**
     * Function to translate plain English values into custom data keys
     * used by Civi (custom_*)
     * @param  array $array  the array to operate on
     * @access protected
     */
    protected function createCiviKeys(&$array) {
        if (isset($this->custom_data))
            foreach ($this->custom_data as $friendly_key => $civi_key)
                if (isset($array[$friendly_key]))
                    $array[$civi_key] = $array[$friendly_key];
    }

    /**
     * Function to translate custom data keys (custom_*) into 
     * plain English values
     * @param  array $array  the array to operate on
     * @access protected
     */
    protected function createFriendlyKeys(&$array) {
        if (isset($this->custom_data))
            foreach ($this->custom_data as $friendly_key => $civi_key)
                if (isset($array[$civi_key]))
                    $array[$friendly_key] = $array[$civi_key];
    }

    /**
     * Function for setting errors - is basically just a wrapper for watchdog() at the moment
     * but could be extended in future to post errors differently .. without having to change 
     * every line of error setting code in the data model classes
     * @param string $message  an error message to be posted to the log
     * @param array  $params   optional placeholder replacements
     */
    protected function error($message, $params) {
        return watchdog('av_volunteer', $message, $params, WATCHDOG_ERROR);
    } 

    /**
     * Force child classes to implement a load method
     * @access public
     * @abstract
     */
    abstract public function load();

    /**
     * Debugging function - log the value of a variable using watchdog()
     * @param  string $message  identifying message
     * @param  string $var      the variable to log
     * @access public
     */
    public function log($message, $var) {
        return watchdog('av_volunteer', "$message = <pre>" . print_r($var, true) . '</pre>'); 
    }

    /**
     * Utility function to resolve a Drupal user id to a Civi contact id
     * @param  int $uid
     * @return int
     * @access protected
     */
    protected function getContactID($uid) {

        try {

            $contact_id = civicrm_api3('UFMatch', 'getvalue', array(
                'uf_id'  => $uid,
                'return' => 'contact_id'
            ));

        } catch (CiviCRM_API3_Exception $e) { 
            return $this->error("Error getting contact id for uid @uid: @excuse", array(
                '@uid'    => $uid,
                '@excuse' => $e->getMessage()
            ));
        }

        if (!$contact_id) {
            return $this->error("Unable to find matching contact id for uid @uid", array(
                '@uid' => $uid
            ));           
        }

        return (int)$contact_id;

    }

    /**
     * Utility function to get option value lists for populating selects
     * @param  string $identifier  the machine name of the option
     * @return array  an array of id => label pairs
     * @access public
     */
    public function getOptions($identifier) {
        
        $options = array();

        if ($identifier == 'state_province') {

            $dao = CRM_Core_DAO::executeQuery("
                SELECT id, name FROM civicrm_state_province WHERE country_id = 1226
            ");
            while ($dao->fetch())
                $options[$dao->id] = $dao->name;

            return $options;

        }

        # get option group id
        try {
            
            $option_group_id = civicrm_api3('OptionGroup', 'getvalue', array(
                'name'   => $identifier,
                'return' => 'id'
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error retrieving option group '@identifier': @excuse", array(
                '@identifier' => $identifier,
                '@excuse'     => $e->getMessage()
            ));           
        }

        # get all option group values
        try {

            $result = civicrm_api3('OptionValue', 'get', array(
                'option_group_id' => $option_group_id,
                'return.label'    => 1,
                'return.value'    => 1
            ));

        } catch (CiviCRM_API3_Exception $e) { 
            return $this->error("Error retrieving option value list for @identifier: @excuse", array(
                '@identifier' => $uid,
                '@excuse'     => $e->getMessage()
            ));
        }
    
        # create key => value list of options from api result
        foreach ($result['values'] as $option)
            $options[$option['value']] = $option['label'];

        return $options;

    }

    /**
     * Force child classes to implement a save method
     * @access public
     * @abstract
     */
    abstract public function save();

    /**
     * Function for setting warnings - like error(), but for warnings
     * @param  string $message  an error message to be posted to the log
     * @param  array  $params   optional placeholder replacements
     * @access protected
     */
    protected function warning($message, $params) {
        return watchdog('av_volunteer', $message, $params, WATCHDOG_WARNING);
    } 

}