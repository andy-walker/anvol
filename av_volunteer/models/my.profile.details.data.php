<?php

/**
 * Data model class for My Profile -> Details form
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
        'charity_type'         => 'custom_11',
        'charity_no'           => 'custom_69',
        'charity_desc'         => 'custom_21',
        'where_hear_charity'   => 'custom_82',
        'newsletter'           => 'custom_85',
        'email_opps'           => 'custom_84',
        'reason'               => 'custom_18',
        'where_hear_volunteer' => 'custom_71',
        'sharing'              => 'custom_20',
        'assistance_type'      => 'custom_86',
        'region'               => 'custom_87'
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

        # if charity rep, load related contact information for charity
        if (av_role_has_roles('Charity Rep'))
            return $this->loadCharityDetails();

        # if volunteer, load data for volunteer version of the form
        if (av_role_has_roles('Individual Volunteer'))
            return $this->loadVolunteerDetails();

        # if regional co-ordinator, load data for regional co-ordinator version of the form
        if (av_role_has_roles('Regional Co-ordinator'))
            return $this->loadRegionalCoordinatorDetails();

    }

    protected function loadCharityDetails() {

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

        if (!in_array('CharityRep', $contact_sub_type)) {
            drupal_set_message(t("Unable to load charity data: Contact is not of type 'CharityRep'"), 'error');
            return $this->error("Error loading charity information for contact id: @contact_id - contact not of type 'CharityRep'", array(
                '@contact_id' => $this->contact_id
            ));
        }

        # get relationships
        try {
            $result = civicrm_api3('relationship', 'get', array(
                'contact_id_a'         => $this->contact_id,
                'relationship_type_id' => 12, # Main contact for
                'is_active'            => 1
            ));
        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting related charity for @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        if (!$result['count']) {
            drupal_set_message(t("Unable to load charity data: No active 'Main Contact' relationship."), 'error');
            return $this->error("Unable to load charity information for contact id: @contact_id - no related contact.", array(
                '@contact_id' => $this->contact_id
            ));               
        }

        $relationship = reset($result['values']);
        $custom       = &$this->custom_data;

        try {
            
            $this->charity = civicrm_api3('contact', 'getsingle', array(
                'id'                       => $relationship['contact_id_b'],
                'return.organization_name' => 1
            ) + array(
                'return.' . $custom['charity_type']       => 1,
                'return.' . $custom['charity_no']         => 1,
                'return.' . $custom['charity_desc']       => 1,
                'return.' . $custom['where_hear_charity'] => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error loading charity for @contact_id: @excuse", array(
                '@contact_id' => $relationship['contact_id_b'],
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->charity);
        $this->log('charity', $this->charity);

    }

    protected function loadVolunteerDetails() {

        # get contact-related custom data 
        try {

            $this->contact = civicrm_api3('contact', 'getsingle', array(
                'id' => $this->contact_id
            ) + array(
                'return.' . $this->custom_data['newsletter']           => 1,
                'return.' . $this->custom_data['email_opps']           => 1,
                'return.' . $this->custom_data['reason']               => 1,
                'return.' . $this->custom_data['where_hear_volunteer'] => 1,
                'return.' . $this->custom_data['sharing']              => 1,
                'return.' . $this->custom_data['assistance_type']      => 1
            ));

        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting contact data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->contact);

        $this->log('volunteer', $this->contact);

    }

    protected function loadRegionalCoordinatorDetails() {

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

        if (!in_array('Regional_Coordinator', $contact_sub_type)) {
            drupal_set_message(t("Unable to load regional co-ordinator data: Contact is not of type 'Regional Co-ordinator'"), 'error');
            return $this->error("Error loading regional co-ordinator information for contact id: @contact_id - contact not of type 'Regional Co-ordinator'", array(
                '@contact_id' => $this->contact_id
            ));
        }

        # load custom data for co-ordinator
        try {
            $this->contact = civicrm_api3('contact', 'getsingle', array(
                'id' => $this->contact_id
            ) + array(
                'return.' . $this->custom_data['region'] => 1
            ));
        } catch (CiviCRM_API3_Exception $e) {
            return $this->error("Error getting regional co-ordinator data for contact id @contact_id: @excuse", array(
                '@contact_id' => $this->contact_id,
                '@excuse'     => $e->getMessage()
            ));
        }

        $this->createFriendlyKeys($this->contact);

        $this->contact['region'] = str_replace(
            array('(', ')'), '', end(explode(' ', $this->contact['region']))
        );

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

        $success = false;

        # save charity details, if present
        if (av_role_has_roles('Charity Rep')) {
            
            $success = true;
            $this->createCiviKeys($this->charity);

            try {
                civicrm_api3('contact', 'create', $this->charity);
            } catch (CiviCRM_API3_Exception $e) {
                $success = $this->error("Error saving charity data for charity id @contact_id: @excuse !debug", array(
                    '@contact_id' => $this->charity['id'],
                    '@excuse'     => $e->getMessage(),
                    '!debug'      => $debug($this->charity)
                ));
            }

        } elseif (av_role_has_roles('Regional Co-ordinator')) {
            
            if ($tid = $this->contact['region']) {

                $county = taxonomy_term_load($tid);
                $region = reset(taxonomy_get_parents($tid));
                
                $this->contact['region'] = sprintf('%s » %s (%d)',
                    $region->name, $county->name, $county->tid
                );
                
                $custom = &$this->custom_data;
                $params = array(
                    'entityID'        => $this->contact['id'],
                    $custom['region'] => $this->contact['region']
                );

                CRM_Core_BAO_CustomValueTable::setValues($params);
                $success = true;

            }

        } else {

            $success = true;

            $this->createCiviKeys($this->contact);
            $this->log('saving contact', $this->contact);

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

        } 

        return (bool)$success;

    }

}