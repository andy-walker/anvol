<?php

function av_volunteer_my_profile($form, $form_state) {
    
    global $user;

    $module_path = drupal_get_path('module', 'av_volunteer');
    drupal_add_css($module_path . '/css/base.css');
    drupal_add_js($module_path . '/js/profile.js');

    $days = array(
        'mon' => '',
        'tue' => '',
        'wed' => '',
        'thu' => '',
        'fri' => '',
        'sat' => '',
        'sun' => ''
    );

    $form['signup_date'] = array(
        '#type'   => 'item',
        '#title'  => t('Signup Date') . ':',
        '#markup' => '&lt;signup date here&gt;'
    );

    $form['title'] = array(
        '#type' => 'select',
        '#title' => t('Title'),
        '#options' => array( 
            0 => t('Mr'), 
            1 => t('Mrs'), 
            2 => t('Miss') 
        ),
    );
        
    $form['firstname'] = array(
        '#type'  => 'textfield',
        '#title' => t('First Name'),
        '#size'  => 31
    );
        
    $form['lastname'] = array(
        '#type'  => 'textfield',
        '#title' => t('Last Name'),
        '#size'  => 31
    );

    if (_av_volunteer_is_a('Individual Volunteer')) {

        $form['known_as'] = array(
            '#type'  => 'textfield',
            '#title' => t('Known As'),
            '#size'  => 31
        );

    }

    $form['address'] = array(
        '#type'  => 'textfield',
        '#title' => t('Address'),
        '#size'  => 70
    );

    if (_av_volunteer_is_a('Charity Rep', 'Company Rep')) {

        $form['city'] = array(
            '#type'  => 'textfield',
            '#title' => t('City / Town'),
            '#size'  => 31
        );

        $form['borough'] = array(
            '#type'  => 'textfield',
            '#title' => t('London / Borough'),
            '#size'  => 31
        );

        $form['county'] = array(
            '#type'    => 'select',
            '#title'   => t('County'),
            '#options' => array(0 => t('Select...')) + _av_volunteer_get_counties()
        );

    }

    $form['postcode'] = array(
        '#type'  => 'textfield',
        '#title' => t('Postcode'),
        '#size'  => 31
    );

    $form['phone_number'] = array(
        '#type'  => 'textfield',
        '#title' => t('Phone Number'),
        '#size'  => 31
    );

    $form['mobile_number'] = array(
        '#type'  => 'textfield',
        '#title' => t('Mobile Number'),
        '#size'  => 31
    );

    $form['email'] = array(
        '#type'  => 'textfield',
        '#title' => t('Email'),
        '#size'  => 70
    );

    # status - relevant to Individual Volunteer only
    if (_av_volunteer_is_a('Individual Volunteer', 'website admin', 'Assessor')) {

        $form['status'] = array(
            '#type'          => 'radios',
            '#title'         => 'Status',
            '#default_value' => 1,
            '#options'       => (
                # volunteer only sees Available / Unavailable, admin / assessor sees all statuses
                _av_volunteer_is_a('website admin', 'Assessor') ? array(
                    1 => t('Potential'),
                    2 => t('Available'),
                    3 => t('Cancelled'),
                    4 => t('Unavailable'),
                    5 => t('Resigned')
                ) : array(
                    2 => t('Available'),
                    4 => t('Unavailable')
                )
            )
        );

    }

    # best time to contact - Individual Volunteer and Client Rep
    if (_av_volunteer_is_a('Individual Volunteer', 'Company Rep', 'website admin', 'Assessor')) {

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

        $form['best_time_morning'] = array(
            '#type'    => 'checkboxes',
            '#options' => $days,
            '#prefix'  => '<div class="checkbox-row"><div class="checkbox-cell row-label">Morning</div>',
            '#suffix'  => '</div>'
        );

        $form['best_time_afternoon'] = array(
            '#type'    => 'checkboxes',
            '#options' => $days,
            '#prefix'  => '<div class="checkbox-row"><div class="checkbox-cell row-label">Afternoon</div>',
            '#suffix'  => '</div>'
        );

        $form['best_time_evening'] = array(
            '#type'    => 'checkboxes',
            '#options' => $days,
            '#prefix'  => '<div class="checkbox-row"><div class="checkbox-cell row-label">Evening</div>',
            '#suffix'  => '</div>'
        );

        $form['best_time_post'] = array(
            '#markup' => '<div style="clear:both"></div></div>'
        );

    }

    $form['submit'] = array(
        '#type'  => 'submit',
        '#value' => t('Save')
    );

    return $form;

}

function av_volunteer_my_profile_details_tab($form, $form_state) {

    # for charity reps, display a different form here
    if (av_role_has_roles('Charity Rep'))
        return av_volunteer_my_profile_details_tab_charity($form, $form_state);

    $yes_no = array(
        'yes' => t('Yes'),
        'no'  => t('No')
    );

    $form += array(

        'work_type' => array(
            '#type'    => 'checkboxes',
            '#title'   => ' ',
            '#options' => array(
                1 => t('I want to help disabled people at home'),
                2 => t('I want to help charities')
            )
        ),
    
        'monthly_newsletter' => array(
            '#type'    => 'radios',
            '#options' => $yes_no,
            '#title'   => t('Monthly Newsletter')
        ),

        'email_opportunities' => array(
            '#type'    => 'radios',
            '#options' => $yes_no,
            '#title'   => t('Email Opportunities')
        ),

        'share_details'    => array(
            '#type'        => 'radios',
            '#options'     => array(
                    0 => t('Brief Details'),
                    1 => t('Full Details')
                ),
            '#title'       => t('Share Details')
        ),

        'where_you_hear' => array(
            '#type'    => 'select',
            '#title'   => t('Where did you hear about us?'),
            '#options' => array(
                0 => t('Select ...'),
                1 => t('Personal Recommendation'),
                2 => t('Press article'),
                3 => t('Exhibition or Event'),
                4 => t('AbilityNet Event'),
                5 => t('Volunteer centre or charity'),
                6 => t('Volunteer'),
                7 => t('Previously used Service'),
                8 => t('ITC'),
                9 => t('Small Charities Coalition'),
                10 => t('Web search'),
                11 => t('LASA'),
                12 => t('Technology Trust'),
                13 => t('TT Exchange'),
                14 => t('Infrastructure body'),
                15 => t('Company Scheme'),
                16 => t('BCS'),
                17 => t('AbilityNet Website'),
                18 => t('Other')
            )
        ),

        'why_you_want_do_this' => array(
            '#type'  => 'textarea',
            '#title' => t('Why do you want to volunteer with us?')
        ),

        'comment' => array(
            '#type'  => 'textarea',
            '#title' => t('Comment')
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        ),

    );

    return $form;

}

function av_volunteer_my_profile_details_tab_charity($form, $form_state) {

    $yes_no = array(
        'yes' => t('Yes'),
        'no'  => t('No')
    );

    return $form + array(

        'work_type' => array(
            '#type'    => 'checkboxes',
            '#title'   => ' ',
            '#options' => array(
                1 => t('I want to help disabled people at home'),
                2 => t('I want to help charities')
            )
        ),
    
        'monthly_newsletter' => array(
            '#type'    => 'radios',
            '#options' => $yes_no,
            '#title'   => t('Monthly Newsletter')
        ),

        'email_opportunities' => array(
            '#type'    => 'radios',
            '#options' => $yes_no,
            '#title'   => t('Email Opportunities')
        ),

        'share_details' => array(
            '#type'    => 'radios',
            '#options' => array(
                    0 => t('Brief Details'),
                    1 => t('Full Details')
                ),
            '#title'   => t('Share Details')
        ),

        'charity_type' => array(
            '#type'  => 'textfield',
            '#title' => t('Type'),
            '#size'  => 31
        ),

        'charity_no' => array(
            '#type'  => 'textfield',
            '#title' => t('Registered Charity Number'),
            '#size'  => 31
        ),

        'charity_description' => array(
            '#type'  => 'textarea',
            '#title' => t('Description of Charity')
        ),

        'umbrella' => array(
            '#type'  => 'textfield',
            '#title' => t('Umbrella'),
            '#size'  => 31
        ),

        'where_you_hear' => array(
            '#type'    => 'select',
            '#title'   => t('Where did you hear about us?'),
            '#options' => array(
                0 => t('Select ...'),
                1 => t('Personal Recommendation'),
                2 => t('Press article'),
                3 => t('Exhibition or Event'),
                4 => t('AbilityNet Event'),
                5 => t('Volunteer centre or charity'),
                6 => t('Volunteer'),
                7 => t('Previously used Service'),
                8 => t('ITC'),
                9 => t('Small Charities Coalition'),
                10 => t('Web search'),
                11 => t('LASA'),
                12 => t('Technology Trust'),
                13 => t('TT Exchange'),
                14 => t('Infrastructure body'),
                15 => t('Company Scheme'),
                16 => t('BCS'),
                17 => t('AbilityNet Website'),
                18 => t('Other')
            )
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        )

    );

}

function av_volunteer_my_profile_skills_tab($form, $form_state) {

    module_load_include('inc', 'av_volunteer', 'av_volunteer.settings');

    # add jstree library for skills treeview popup
    $module_path = drupal_get_path('module', 'av_volunteer');
    
    drupal_add_css($module_path . '/js/jstree/themes/default/style.min.css');
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
            '#type'    => 'radios',
            '#title'   => t('Highest level of academic achievement'),
            '#options' => array(
                1 => t('Doctorates'),
                2 => t('Masters degrees, postgraduate certificates and diplomas'),
                3 => t('Bachelor degrees, graduate certificates and diplomas'),
                4 => t('Diplomas of higher education and further education, foundation degrees and higher national diplomas'),
                5 => t('Certificates of higher education')
            )
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

    $form['CV']['upload'] = array(
        '#type'  => 'file',
        '#title' => t('Upload CV')
    );

    return $form;

}

function av_volunteer_my_profile_references_tab($form, $form_state) {

    drupal_add_css(drupal_get_path('module', 'av_volunteer') . '/css/base.css');

    $num_references = 2;

    foreach (range(0, --$num_references) as $index) {
        
        $form['reference_name' . $index] = array(
            '#type'  => 'textfield',
            '#title' => t('Reference Name'),
            '#size'  => 40
        );        
        
        $form['reference_phone' . $index] = array(
            '#type'  => 'textfield',
            '#title' => t('Phone No'),
            '#size'  => 21
        );

        $form['reference_address' . $index] = array(
            '#type'  => 'textfield',
            '#title' => t('Reference Address'),
            '#size'  => 70
        );      

        if (av_role_has_roles('website admin', 'Assessor'))
            $form['reference_ok' . $index] = array(
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

    return $form;
}

function av_volunteer_my_profile_password_tab($form, $form_state) {

    return $form + array(
        
        'old_password' => array(
            '#type'  => 'password',
            '#title' => t('Old Password'),
            '#size'  => 31
        ),
                
        'confirm_password' => array('#type' => 'password_confirm'),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        )
    
    );

}
