<?php

/**
 * Message template datamodel class
 * andyw@circle, 01/06/2014
 */
class Opportunity_Message_Template extends Opportunity_Abstract_Model {
  
    public $template_group = 'msg_tpl_workflow_uf'; # option group name - this will probably always be msg_tpl_workflow_uf
    public $template_name;                          # option value name in civicrm_option_value

    public $label;           # the label the user sees
    public $subject;         # the subject of the mail
    public $html;            # html body of mail
    public $text;            # plain text body of mail

    public $id;              # id on civicrm_msg_template table
    public $workflow_id;     # workflow_id - is the option value id in civicrm_option_value, also pseudo-fk on civicrm_msg_template

    # We'll eventually get into a situation where they've updated half the templates on the system, but are still requesting more
    # get added, as we did with someone else before. When set to false (the default), this prevents overwrite of existing 
    # templates - will only add new ones
    public $overwrite = false;       

    /**
     * Get the maximimum value of 'value' for the specified option_group
     * @param  $group_id (integer) - the option_group_id of option_value set
     * @return int
     */
    protected function getOptionValueMax($group_id) {
        
        return CRM_Core_DAO::singleValueQuery(
            "SELECT MAX(CAST(value AS UNSIGNED)) FROM civicrm_option_value WHERE option_group_id = %1",
            array(
               1 => array($group_id, 'Positive')
            )
        );

    }

    /**
     * Get the option group id that corresponds the the group_name passed in
     * @param $group_name (string) - the option group name
     * @return int - the option group id
     */
    protected function getOptionGroupID($group_name) {
        
        # cache results
        static $group_ids = array();
        
        # return from cache, if we have an entry
        if (isset($group_ids[$group_name]))
            return $group_ids[$group_name];
        
        # otherwise query db, set cache entry and return it
        return $group_ids[$group_name] = CRM_Core_DAO::singleValueQuery("
            SELECT id from civicrm_option_group WHERE name = %1
        ", array(
              1 => array($group_name, 'String')
           )
        );

    }

    /**
     * Model load method
     * @param  $properties (array) (optional) properties to set before saving
     * @return bool (success)
     */
    public function load($properties = array()) {

        # clear any previous error messages
        $this->clearErrors();

        # set any properties that were passed in
        $this->set($properties);

        # construct select from and joins
        $query = "
                SELECT tpl.*, ov.name AS template_name, og.name AS template_group
                  FROM civicrm_msg_template tpl
            INNER JOIN civicrm_option_value ov ON ov.id = tpl.workflow_id
            INNER JOIN civicrm_option_group og ON og.id = ov.option_group_id
        ";

        # can load from id, workflow_id, or via a combination of template_group and template_name.
        # construct relevant WHERE clause accordingly ..
        switch (true) {
            
            # load via id
            case !empty($this->id):
                
                $query .= "WHERE tpl.id = %1";
                $params = array(
                    1 => array($this->id, 'Positive')
                );
                break;

            # load via workflow id
            case !empty($this->workflow_id):
                $query .= "WHERE tpl.workflow_id = %1";
                $params = array(
                    1 => array($this->workflow_id, 'Positive')
                );
                break;

            # load via template_group / template_name
            case !empty($this->template_group) and !empty($this->template_name):
                $query .= "WHERE og.name = %1 AND ov.name = %2";
                $params = array(
                    1 => array($this->template_group, 'String'),
                    2 => array($this->template_name,  'String')
                );
                break;

            # when we don't have anything sensible we can load from, log error and return false
            default:
                $this->error(
                    "No valid params to load model from, requires 'id', 'workflow_id' " . 
                    "or 'template_group' + 'template_name'"
                );
                return false;

        }

        $dao = CRM_Core_DAO::executeQuery($query, $params);
        
        # if no result, that's ok - return false, no error
        if (!$dao->fetch())
            return false;

        # load internal properties from the results of the query
        $this->template_group = $dao->template_group;
        $this->template_name  = $dao->template_name;
        $this->id             = $dao->id;
        $this->label          = $dao->msg_title;
        $this->subject        = $dao->msg_subject;
        $this->msg_text       = $dao->msg_text;
        $this->msg_html       = $dao->msg_html;
        $this->workflow_id    = $dao->workflow_id;
        
        # and return true (success)
        return true;
    
    }

    /**
     * Model save method
     * @param  $properties (array) (optional) properties to set before saving
     * @return bool (success)
     */
    public function save($properties = array()) {
        
        # clear any previous error messages
        $this->clearErrors();

        # set any properties that were passed in
        $this->set($properties);

        # if load failed, insert template as new record
        if (!$this->load()) {

            # validate model
            if (!$this->validate())
                return false;

            # get the option group id we should be using - we're parenting all message templates
            # to msg_tpl_workflow_uf, then they should magically appear in the ui and be editable.
            if (!$group_id = $this->getOptionGroupID('msg_tpl_workflow_uf')) {
                $this->error("Unable to get group id for 'msg_tpl_workflow_uf'");
                return false;
            }
            
            # get max value 'value' field on opt group, so we can append our template after that
            $max_value = $this->getOptionValueMax($group_id);

            # insert option value
            $result = civicrm_api('OptionValue', 'create', array(
                'version'         => 3,
                'option_group_id' => $group_id,
                'label'           => $this->label,
                'name'            => $this->template_name,
                'value'           => $max_value + 1,
                'weight'          => $max_value + 1,
                'is_reserved'     => 0,
                'is_active'       => 1
            ));

            if ($result['is_error']) {
                $this->error($result['error_message']);
                return false;
            }

            $value = reset($result['values']);
            $this->workflow_id = $value['id'];

            # insert message template record
            CRM_Core_DAO::executeQuery("
                INSERT INTO civicrm_msg_template (
                    id, msg_title, msg_subject, msg_text, msg_html, is_active, workflow_id, is_default, is_reserved
                ) VALUES (
                    NULL, %1, %2, %3, %4, 1, %5, 0, 0
                )
            ", array(
                  1 => array($this->label,       'String'),
                  2 => array($this->subject,     'String'),
                  3 => array($this->text,        'String'),
                  4 => array($this->html,        'String'),
                  5 => array($this->workflow_id, 'Positive')
               )
            );

            # populate the model's id property with the inserted id
            $this->id = CRM_Core_DAO::singleValueQuery('SELECT LAST_INSERT_ID()');

            # return success
            return true;

        }

        # load was successful, update existing template record

        # validate model
        if (!$this->validate())
            return false;

        # abort update when overwrite flag set to true
        if (!$this->overwrite) {
            if (function_exists('drush_log'))
                drush_log(t('Skipping as record already exists. Use --force=1 to overwrite'), 'warning');
            return true;
        }

        CRM_Core_DAO::executeQuery("
            UPDATE civicrm_msg_template 
               SET msg_title   = %1,
                   msg_subject = %2,
                   msg_text    = %3,
                   msg_html    = %4
             WHERE id = %5 
        ", array(
              1 => array($this->label,   'String'),
              2 => array($this->subject, 'String'),
              3 => array($this->text,    'String'),
              4 => array($this->html,    'String'),
              5 => array($this->id,      'Positive')
           )
        );

        # return success
        return true; # todo: catch errors in db queries

    }

    /**
     * Model validate method - check model properties are 
     * present and ok. Should be called before saving to db.
     * @return bool (success)
     */
    protected function validate() {

        # check for empty req'd properties
        foreach (array(
            'template_group', 
            'template_name', 
            'subject', 
            'text', 
            'html'
        ) as $property)
            if (empty($this->$property))
                $this->error(
                    "Failed validation: '@property' must be set.",
                    array(
                        '@property' => $property
                    )
                );

        return !$this->isError();

    }

}