<?php

function av_volunteer_my_profile($form_state) {
    
    civicrm_initialize();
    global $user;

    # add styles + scripts
    $module_path = drupal_get_path('module', 'av_volunteer');
    drupal_add_css($module_path . '/css/base.css');
    drupal_add_js($module_path . '/js/profile.js');

    # load data
    $data = new My_Profile_Contact_Data($user->uid);

    $days = array(
        'mon' => '',
        'tue' => '',
        'wed' => '',
        'thu' => '',
        'fri' => '',
        'sat' => '',
        'sun' => ''
    );

    if (av_role_has_roles('Individual Volunteer', 'website admin')) {
        $form['volunteer_id'] = array(
            '#type'   => 'item',
            '#title'  => t('Volunteer ID') . ':',
            '#markup' => $data->contact['id']
        );
    }

    $form['signup_date'] = array(
        '#type'   => 'item',
        '#title'  => t('Signup Date') . ':',
        '#markup' => date('d/m/Y', $data->user->created)
    );

    $form['title'] = array(
        '#type'          => 'select',
        '#title'         => t('Title'),
        '#options'       => array(0 => 'Select...') + $data->getOptions('individual_prefix'),
        '#default_value' => $data->contact['prefix_id']
    );
        
    $form['firstname'] = array(
        '#type'          => 'textfield',
        '#title'         => t('First Name'),
        '#size'          => 31,
        '#required'      => true,
        '#default_value' => isset($data->contact['first_name']) ? $data->contact['first_name'] : ''
    );
        
    $form['lastname'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Last Name'),
        '#size'          => 31,
        '#required'      => true,
        '#default_value' => isset($data->contact['last_name']) ? $data->contact['last_name'] : ''
    );

    if (av_role_has_roles('Individual Volunteer', 'website admin', 'IT4C Administrator')) {

        $form['known_as'] = array(
            '#type'          => 'textfield',
            '#title'         => t('Known As'),
            '#size'          => 31,
            '#default_value' => isset($data->contact['nick_name']) ? $data->contact['nick_name'] : ''
        );

    }

    $form['address'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Address'),
        '#size'          => 70,
        '#required'      => true,
        '#default_value' => isset($data->address['street_address']) ? $data->address['street_address'] : ''
    );

    if (av_role_has_roles('Charity Rep', 'Company Rep')) {

        $form['city'] = array(
            '#type'          => 'textfield',
            '#title'         => t('City / Town'),
            '#size'          => 31,
            '#default_value' => isset($data->address['city']) ? $data->address['city'] : ''
        );

        $form['borough'] = array(
            '#type'  => 'textfield',
            '#title' => t('London / Borough'),
            '#size'  => 31
        );

        $form['county'] = array(
            '#type'          => 'select',
            '#title'         => t('County'),
            '#options'       => array(0 => t('Select...')) + $data->getOptions('state_province'),
            '#default_value' => isset($data->address['state_province_id']) ? $data->address['state_province_id'] : 0
        );

    }

    $form['postcode'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Postcode'),
        '#size'          => 31,
        '#required'      => true,
        '#default_value' => isset($data->address['postal_code']) ? $data->address['postal_code'] : ''
    );

    $form['phone_number'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Phone Number'),
        '#size'          => 31,
        '#required'      => true,
        '#default_value' => isset($data->phone['main']['phone']) ? $data->phone['main']['phone'] : ''
    );

    $form['alternative_phone_number'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Alternative Phone Number'),
        '#size'          => 31,
        '#default_value' => isset($data->phone['alternative']['phone']) ? $data->phone['alternative']['phone'] : ''
    );

    $form['mobile_number'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Mobile Number'),
        '#size'          => 31,
        '#default_value' => isset($data->phone['mobile']['phone']) ? $data->phone['mobile']['phone'] : ''
    );

    $form['email'] = array(
        '#type'          => 'textfield',
        '#title'         => t('Email'),
        '#size'          => 70,
        '#required'      => true,
        '#default_value' => isset($data->email['email']) ? $data->email['email'] : ''
    );

    if (av_role_has_roles('Individual Volunteer', 'website admin', 'IT4C Administrator')) {

        # status - relevant to Individual Volunteer only
        $statuses = $data->getOptions('volunteer_status_20140625125929');
        
        if (!av_role_has_roles('website admin', 'IT4C Administrator'))
            unset($statuses['Potential'], $statuses['Cancelled'], $statuses['Resigned']);

        $form['status'] = array(
            '#type'          => 'radios',
            '#title'         => t('Status'),
            '#default_value' => isset($data->contact['status']) ? $data->contact['status'] : 0,
            '#options'       => $statuses
        );

    }

    # best time to contact - Individual Volunteer and Client Rep
    if (av_role_has_roles('Individual Volunteer', 'Company Rep', 'website admin')) {

        $form['best_time_pre'] = array(
            '#markup' => '<div class="best-time-label">' . t('Your Availability') . '</div>' . "\n"
                       . '<div class="checkbox-table">' . "\n"
                       . '  <div class="checkbox-header-row">' . "\n"
                       . '    <div class="checkbox-cell row-label">&nbsp;</div>' . "\n"
                       . '    <div class="checkbox-header">Mon</div>' . "\n"
                       . '    <div class="checkbox-header">Tue</div>' . "\n"
                       . '    <div class="checkbox-header">Wed</div>' . "\n"
                       . '    <div class="checkbox-header">Thu</div>' . "\n"
                       . '    <div class="checkbox-header">Fri</div>' . "\n"
                       . '    <div class="checkbox-header">Sat</div>' . "\n"
                       . '    <div class="checkbox-header round-right">Sun</div>' . "\n"
                       . '  </div>' . "\n" 
        );

        if (isset($data->contact['availability']))
            $best_time = _av_volunteer_convert_availability_from_text(
                $data->contact['availability']
            );

        watchdog('andyw', 'best_time = <pre>' . print_r($best_time, true) . '</pre>');

        $form['best_time_morning'] = array(
            '#type'          => 'checkboxes',
            '#options'       => $days,
            '#prefix'        => '<div class="checkbox-row"><div class="checkbox-cell row-label">Morning</div>',
            '#suffix'        => '</div>',
            '#default_value' => isset($best_time['morning']) ? $best_time['morning'] : array()
        );

        $form['best_time_afternoon'] = array(
            '#type'          => 'checkboxes',
            '#options'       => $days,
            '#prefix'        => '<div class="checkbox-row"><div class="checkbox-cell row-label">Afternoon</div>',
            '#suffix'        => '</div>',
            '#default_value' => isset($best_time['afternoon']) ? $best_time['afternoon'] : array()
        );

        $form['best_time_evening'] = array(
            '#type'          => 'checkboxes',
            '#options'       => $days,
            '#prefix'        => '<div class="checkbox-row"><div class="checkbox-cell row-label">Evening</div>',
            '#suffix'        => '</div>',
            '#default_value' => isset($best_time['evening']) ? $best_time['evening'] : array()
        );

        $form['best_time_post'] = array(
            '#markup' => '<div style="clear:both"></div></div>'
        );

    }

    $form['submit'] = array(
        '#type'        => 'submit',
        '#value'       => t('Save'),
        '#validate'    => array('av_volunteer_my_profile_form_validate')
    );

    ;
    $form['#submit'][]   = 'av_volunteer_my_profile_form_submit';

    return $form;

}

/**
 * Form validation handler for My Profile -> Contact form
 */
function av_volunteer_my_profile_form_validate($form, &$form_state) {

    # check phone numbers are valid phone numbers - can begin with a '+' followed by numbers and/or spaces
    # must contain at least one numeric value - accept empty on non req'd
    foreach (array('phone_number', 'alternative_phone_number', 'mobile_number') as $field)
        if (trim($form_state['values'][$field]))
            if (!preg_match("/^\+?[0-9 ]+$/", $form_state['values'][$field]))
                form_set_error($field, t("'@field_name' is not a valid phone number", array(
                    '@field_name' => $form[$field]['#title']
                )));
    
    # validate email
    if (!filter_var($form_state['values']['email'], FILTER_VALIDATE_EMAIL))
        form_set_error($field, t("'@field_name' is not a valid email address.", array(
            '@field_name' => $form['email']['#title']
        )));


}

/**
 * Form submit handler for My Profile -> Contact form
 */
function av_volunteer_my_profile_form_submit($form, &$form_state) {

    watchdog('andyw', 'form_state[values] = <pre>' . print_r($form_state['values'], true) . '</pre>');
   
    global $user;
    $data = new My_Profile_Contact_Data($user->uid);

    $data->contact['prefix_id']          = $form_state['values']['title'];
    $data->contact['first_name']         = $form_state['values']['firstname'];
    $data->contact['last_name']          = $form_state['values']['lastname'];
    $data->contact['nick_name']          = $form_state['values']['known_as'];
    $data->contact['status']             = $form_state['values']['status'];
    
    $data->address['street_address']     = $form_state['values']['address'];
    $data->address['city']               = $form_state['values']['city'];
    $data->address['state_province_id']  = $form_state['values']['county'];
    $data->address['postal_code']        = $form_state['values']['postcode'];

    $data->phone['main']['phone']        = $form_state['values']['phone_number'];
    $data->phone['alternative']['phone'] = $form_state['values']['alternative_phone_number'];
    $data->phone['mobile']['phone']      = $form_state['values']['mobile_number'];

    $data->email['email']                = $form_state['values']['email'];

    $data->contact['availability']       = _av_volunteer_convert_availability_to_text(array(
        'morning'   => $form_state['values']['best_time_morning'],
        'afternoon' => $form_state['values']['best_time_afternoon'],
        'evening'   => $form_state['values']['best_time_evening']
    ));

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');

}

function av_volunteer_my_profile_details_tab($form_state) {

    # for charity reps, display a different form here
    if (av_role_has_roles('Charity Rep'))
        return av_volunteer_my_profile_details_tab_charity($form_state);

    # likewise for regional co-ordinators, show them a different form too
    elseif (av_role_has_roles('Regional Co-ordinator'))
        return av_volunteer_my_profile_details_tab_regional_coordinator($form_state);

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    $yes_no = array(
        '1' => t('Yes'),
        '0' => t('No')
    );

    $where_hear_options = array(0 => t('Select ...')) + $data->getOptions('where_did_you_hear_about_us__20140611140531');

    $form = array(

        'work_type' => array(
            '#type'          => 'checkboxes',
            '#title'         => ' ',
            '#options'       => $data->getOptions('type_of_assistance_20140713170230'),
            '#default_value' => isset($data->contact['assistance_type']) ? $data->contact['assistance_type'] : ''
        ),
    
        'monthly_newsletter' => array(
            '#type'          => 'radios',
            '#options'       => $yes_no,
            '#title'         => t('Newsletter'),
            '#default_value' => isset($data->contact['newsletter']) ? $data->contact['newsletter'] : 1
        ),

        'email_opportunities' => array(
            '#type'           => 'radios',
            '#options'        => $yes_no,
            '#title'          => t('Email Opportunities'),
            '#default_value'  => isset($data->contact['email_opps']) ? $data->contact['email_opps'] : 1
        ),

        'share_details' => array(
            '#type'     => 'radios',
            '#title'    => t('Share Details'),
            '#options'       => $data->getOptions('sharing_details_20120112174737'),
            '#default_value' => isset($data->contact['sharing']) ? $data->contact['sharing'] : 'Full details'
        ),

        'where_you_hear' => array(
            '#type'          => 'select',
            '#title'         => t('Where did you hear about us?'),
            '#options'       => $where_hear_options,
            '#default_value' => isset($data->contact['where_hear_volunteer']) ? $data->contact['where_hear_volunteer'] : 0     
        ),

        'why_you_want_do_this' => array(
            '#type'          => 'textarea',
            '#title'         => t('Why do you want to volunteer with us?'),
            '#default_value' => isset($data->contact['reason']) ? $data->contact['reason'] : ''
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        ),

    );

    $form['#submit'][] = 'av_volunteer_my_profile_details_form_submit';

    return $form;

}

function av_volunteer_my_profile_details_form_submit($form, &$form_state) {

    watchdog('andyw', 'running volunteer details submit handler');

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    $data->contact['newsletter']           = $form_state['values']['monthly_newsletter'];
    $data->contact['email_opps']           = $form_state['values']['email_opportunities'];
    $data->contact['reason']               = $form_state['values']['why_you_want_do_this'];
    $data->contact['where_hear_volunteer'] = $form_state['values']['where_you_hear'];
    $data->contact['sharing']              = $form_state['values']['share_details'];
    $data->contact['assistance_type']      = $form_state['values']['work_type'];

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    # more descriptive error will be posted to the watchdog
    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');   

}


function av_volunteer_my_profile_details_tab_charity($form_state) {
    
    civicrm_initialize();

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    $yes_no = array(
        'yes' => t('Yes'),
        'no'  => t('No')
    );

    $where_hear_options = array(0 => t('Select ...')) + $data->getOptions('where_did_you_hear_about_us__20140611140531');

    $form = array(

        'charity_id' => array(
            '#type'   => 'item',
            '#title'  => t('Charity ID') . ':',
            '#markup' => isset($data->charity['id']) ? $data->charity['id'] : ''
        ),

        'charity_name' => array(
            '#type'   => 'item',
            '#title'  => t('Charity Name') . ':',
            '#markup' => isset($data->charity['organization_name']) ? $data->charity['organization_name'] : ''
        ),

        'charity_type' => array(
            '#type'          => 'select',
            '#multiple'      => true,
            '#title'         => t('Type'),
            '#size'          => 5,
            '#options'       => $data->getOptions('charity_type_20120112173143'),
            '#default_value' => isset($data->charity['charity_type']) ? $data->charity['charity_type'] : array()
        ),

        'charity_no' => array(
            '#type'          => 'textfield',
            '#title'         => t('Registered Charity Number'),
            '#size'          => 31,
            '#default_value' => isset($data->charity['charity_no']) ? $data->charity['charity_no'] : ''
        ),

        'charity_description' => array(
            '#type'          => 'textarea',
            '#title'         => t('Description of Charity'),
            '#default_value' => isset($data->charity['charity_desc']) ? $data->charity['charity_desc'] : ''
        ),

        'where_you_hear' => array(
            '#type'          => 'select',
            '#title'         => t('Where did you hear about us?'),
            '#options'       => $where_hear_options,
            '#default_value' => isset($data->charity['where_hear_charity']) ? $data->charity['where_hear_charity'] : 0
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        )

    );

    $form['#submit'][] = 'av_volunteer_my_profile_details_charity_form_submit';

    return $form;

}

function av_volunteer_my_profile_details_charity_form_submit($form, &$form_state) {

    watchdog('andyw', 'running details submit handler');

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    $data->charity['charity_type']       = $form_state['values']['charity_type'];
    $data->charity['charity_no']         = $form_state['values']['charity_no'];
    $data->charity['charity_desc']       = $form_state['values']['charity_description'];
    $data->charity['where_hear_charity'] = $form_state['values']['where_you_hear'];

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    # more descriptive error will be posted to the watchdog
    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');   

}

function av_volunteer_my_profile_details_tab_regional_coordinator($form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    drupal_add_js("
        
        jQuery(function() {
            jQuery('#edit-region option').each(function() {
                if (!jQuery(this).html().match(/^\-/) && jQuery(this).val() != '0')
                    jQuery(this).attr('disabled', 'disabled');
            });
        });

    ", array(
          'type'  => 'inline',
          'scope' => 'header'
       )
    );

    $taxonomy = _av_volunteer_get_regions();

    $form['region'] = array(
        '#type'          => 'select',
        '#title'         => t('Region'),
        '#options'       => array(0 => 'Any') + $taxonomy->terms,
        '#default_value' => isset($data->contact['region']) ? $data->contact['region'] : 0 
    );

    $form['submit'] = array(
        '#type'  => 'submit',
        '#value' => t('Save')
    );

    $form['#submit'][] = 'av_volunteer_my_profile_details_regional_coordinator_form_submit';

    return $form;

}

function av_volunteer_my_profile_details_regional_coordinator_form_submit($form, $form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Details_Data($user->uid);

    $data->contact['region'] = $form_state['values']['region'];

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    # more descriptive error will be posted to the watchdog
    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');  

}

function av_volunteer_my_profile_skills_tab($form, $form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Skills_Data($user->uid);

    module_load_include('inc', 'av_volunteer', 'av_volunteer.settings');

    # add jstree library for skills treeview popup
    $module_path = drupal_get_path('module', 'av_volunteer');
    
    drupal_add_css($module_path . '/js/jstree/dist/themes/default/style.min.css');
    drupal_add_css($module_path . '/css/base.css');
    drupal_add_js($module_path . '/js/jstree/dist/jstree.min.js');
    drupal_add_js($module_path . '/js/skills.js');

    $skills = array();
    foreach (_av_volunteer_get_skills()->sub_level as $skillset)
        foreach ($skillset as $key => $skill)
            $skills['skill' . $key] = $skill;

    drupal_add_js(array(
        'skills' => array(
            'list'         => $skills,
            'nextFreeSlot' => 0
        )
    ), 'setting');

    $form += array(
        
        'achievement_level' => array(
            '#type'          => 'radios',
            '#title'         => t('Highest level of academic achievement'),
            '#options'       => $data->getOptions('level_of_qualification_20140611134004'),
            '#default_value' => isset($data->contact['level_of_qualification']) ? $data->contact['level_of_qualification'] : ''
        ),

        'skills' => array(
            '#type'  => 'fieldset',
            '#title' => t('Experience')
        )

    );

    $form['skills']['add_skills'] = array(
        '#markup' => theme(
            'av_volunteer-skills-treeview',
            array(
                'skills' => _av_volunteer_get_skills()
            )
        )
    );
    
    # assemble options array for years
    $year_options = array(t('Select...'));
    foreach (range(1, 20) as $value)
        $year_options[$value] = $value . ' year' . ($value > 1 ? 's' : '');
    $year_options[20] = '20+';

    foreach (range(0, ABNV_MAX_SKILLS - 1) as $index) {
        
        $form['skills']['skill_wrapper_div_' . $index] = array(
            '#markup' => '<div class="skill-wrapper skill-wrapper-' . $index . '">'
        );

        $form['skills']['skill_' . $index] = array(
            '#type' => 'hidden'
        );

        $form['skills']['skill_delete_' . $index] = array(
            '#markup' => '<button class="btn btn-danger">' . t('Delete') . '</button>'
        );

        $form['skills']['skill_years_' . $index] = array(
            '#type'    => 'select',
            '#options' => $year_options
        );

        $form['skills']['skill_label_' . $index] = array(
            '#markup' => '<div class="skill-label"></div>'
        );

        $form['skills']['skill_wrapper_closediv_' . $index] = array(
            '#markup' => '<div style="clear:both"></div></div>'
        );

    }
    
    $form['CV'] = array(
        '#type'  => 'fieldset',
        '#title' => t('CV')
    );

    if (isset($data->contact['cv']['uri'])) {
        
        $form['CV']['current_file'] = array(
            '#type'   => 'item',
            '#title'  => t('Current file') . ':',
            '#markup' => CRM_Utils_File::cleanFileName($data->contact['cv']['uri'])
        );
        
        $form['CV']['delete_file'] = array(
            '#type'  => 'checkbox',
            '#title' => ts('Delete')
        );

    }

    $form['CV']['cv_upload'] = array(
        '#type'  => 'file',
        '#title' => t('Upload CV')
    );

    $form['submit'] = array(
        '#type'  => 'submit',
        '#value' => t('Save')
    );

    $form['#submit'][] = 'av_volunteer_my_profile_skills_form_submit';

    return $form;

}

function av_volunteer_my_profile_skills_form_submit($form, $form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Skills_Data($user->uid);

    $data->contact['level_of_qualification'] = $form_state['values']['achievement_level'];

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    # more descriptive error will be posted to the watchdog
    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');     

}

function av_volunteer_my_profile_references_tab($form, $form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Reference_Data($user->uid);

    drupal_add_css(drupal_get_path('module', 'av_volunteer') . '/css/base.css');

    $num_references = 2;
    $numbers        = array('One', 'Two', 'Three', 'Four');

    foreach (range(0, --$num_references) as $index) {

        $number = $index + 1;

        $form['reference_' . $index] = array(
            '#type'        => 'fieldset',
            '#title'       => t('Reference @index', array('@index' => $numbers[$index])),
            '#description' => t('(Please provide two references)')
        );
        
        $form['reference_' . $index]['reference_name' . $index] = array(
            '#type'          => 'textfield',
            '#title'         => t('Reference Name'),
            '#size'          => 40,
            '#default_value' => isset($data->contact["reference{$number}_name"]) ? $data->contact["reference{$number}_name"] : ''
        );        
        
        $form['reference_' . $index]['reference_phone' . $index] = array(
            '#type'          => 'textfield',
            '#title'         => t('Phone No'),
            '#size'          => 21,
            '#default_value' => isset($data->contact["reference{$number}_phone"]) ? $data->contact["reference{$number}_phone"] : ''
        );

        $form['reference_' . $index]['reference_address' . $index] = array(
            '#type'  => 'textfield',
            '#title' => t('Reference Address'),
            '#size'  => 70,
            '#default_value' => isset($data->contact["reference{$number}_address"]) ? $data->contact["reference{$number}_address"] : ''
        );      

        if (av_role_has_roles('website admin', 'Assessor'))
            $form['reference_' . $index]['reference_ok' . $index] = array(
                '#type'         => 'checkbox',
                '#field_suffix' => t('Ok'),
            );   

    }

    if (av_role_has_roles('website admin', 'Assessor')) {

        $form['dbs_check'] = array(
            '#type'        => 'fieldset',
            '#title'       => 'DBS Check',
            '#description' => t('(Only required for work at client home)'),
            '#collapsible' => true
        );

        $form['dbs_check']['dbs_completed'] = array(
            '#type'                => 'date_popup',
            '#title'               => t('Date DBS Completed'),
            '#date_label_position' => '',
            '#date_format'         => 'd/m/Y'
        );

        $form['dbs_check']['dbs_became_active'] = array(
            '#type'                => 'date_popup',
            '#title'               => t('Date Became Active'),
            '#date_label_position' => '',
            '#date_format'         => 'd/m/Y',
            '#description'         => t('Date the ID card is sent out')
        );

        $form['dbs_check']['dbs_expiry'] = array(
            '#type'                => 'date_popup',
            '#title'               => t('DBS Expiry Date'),
            '#date_label_position' => '',
            '#date_format'         => 'd/m/Y'
        );

        $form['dbs_check']['dbs_clear'] = array(
            '#markup' => '<div style="clear:both"></div>'
        );

        $form['dbs_check']['dbs_certificate_no'] = array(
            '#type'  => 'textfield',
            '#title' => t('DBS Certificate No'),
            '#size'  => 31
        );

        $form['dbs_check']['dbs_id_no'] = array(
            '#type'  => 'textfield',
            '#title' => t('ID No'),
            '#size'  => 31
        );

    }

    $form['submit'] = array(
        '#type'  => 'submit',
        '#value' => t('Save')
    );

    $form['#submit'][] = 'av_volunteer_my_profile_references_form_submit';

    return $form;
}

function av_volunteer_my_profile_references_form_submit($form, $form_state) {

    civicrm_initialize();

    global $user;
    $data = new My_Profile_Reference_Data($user->uid);

    $data->contact['reference1_name']    = $form_state['values']['reference_name0'];
    $data->contact['reference1_address'] = $form_state['values']['reference_address0'];
    $data->contact['reference1_phone']   = $form_state['values']['reference_phone0'];

    $data->contact['reference2_name']    = $form_state['values']['reference_name1'];
    $data->contact['reference2_address'] = $form_state['values']['reference_address1'];
    $data->contact['reference2_phone']   = $form_state['values']['reference_phone1'];

    if ($data->save()) {
        drupal_set_message(t('Details saved!'));
        return;
    }

    # more descriptive error will be posted to the watchdog
    drupal_set_message(t('There was a problem saving your details. Please contact the site administrator.'), 'error');     

}

function av_volunteer_my_profile_password_tab($form, $form_state) {

    $form = array(
        
        'old_password' => array(
            '#type'  => 'password',
            '#title' => t('Old Password'),
            '#size'  => 31
        ),
                
        'confirm_password' => array('#type' => 'password_confirm'),

        'submit' => array(
            '#type'     => 'submit',
            '#value'    => t('Save'),
            '#validate' => array('av_volunteer_my_profile_password_form_validate')
        )
    
    );

    $form['#submit'][] = 'av_volunteer_my_profile_password_form_submit';

    return $form;

}

function av_volunteer_my_profile_password_form_validate($form, $form_state) {

    global $user;
    if (!user_authenticate($user->name, $form_state['values']['old_password']))
        form_set_error('old_password', t("Old password is invalid"));

}

function av_volunteer_my_profile_password_form_submit($form, $form_state) {

    require_once './includes/password.inc';
    global $user;

    db_update('users')
        ->fields(array(
            'pass' => user_hash_password($form_state['values']['confirm_password'])
        ))
        ->condition('uid', $user->uid)
        ->execute();

    drupal_set_message(t('Password updated!'));

}
