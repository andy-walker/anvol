<?php

function av_volunteer_registration_form($form, $form_state) {

    $module_path = drupal_get_path('module', 'av_volunteer');
    drupal_add_css($module_path . '/css/base.css');
    drupal_add_js($module_path . '/js/registration.js');

    $form += array(

        'signup_type' => array(
            '#type'    => 'radios',
            '#title'   => t('Signup as a'),
            '#options' => array(
                'volunteer' => t('Volunteer'),
                'charity'   => t('Charity')
            ),
            '#required'      => true,
            '#default_value' => 'volunteer' # for now
        ),

        'charity_name' => array(
            '#type'     => 'textfield',
            '#title'    => t('Charity name'),
            '#size'     => 31
        ),
        
        'first_name' => array(
            '#type'     => 'textfield',
            '#title'    => t('First name'),
            '#size'     => 31,
            '#required' => true
        ),

        'last_name' => array(
            '#type'     => 'textfield',
            '#title'    => t('Last name'),
            '#size'     => 31,
            '#required' => true
        ),

        'username' => array(
            '#type'     => 'textfield',
            '#title'    => t('Username'),
            '#size'     => 31,
            '#required' => true
        ),

        'email' => array(
            '#type'     => 'textfield',
            '#title'    => t('Email'),
            '#size'     => 31,
            '#required' => true
        ),

        'mobile' => array(
            '#type'     => 'textfield',
            '#title'    => t('Mobile no'),
            '#size'     => 31,
            '#required' => true
        ),

        'accept_terms' => array(
            '#type'  => 'checkbox',
            '#title' => t(
                'I accept the <a target="_blank" href="@url">terms & conditions</a>',
                array('@url' => url('terms-and-conditions'))
            )
        ),

        # honeypot field - if we end up with a value in this field, we'll know it was filled in by a bot.
        '_hp' => array(
            '#type'  => 'textfield',
            '#title' => t('Please leave this field blank'),
            '#size'  => 15
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Register'),
            '#attributes' => array(
                'class' => array('btn', 'btn-info')
            )
        )
    
    );

    $form['#submit'][] = 'av_volunteer_registration_form_submit';
    return $form;

}

/**
 * Implementation of hook_<form_id>_validate
 */
function av_volunteer_registration_form_validate($form, &$form_state) {
    
    if (trim($form_state['values']['_hp']))
        form_set_error('_hp', t('Please leave last field blank.'));

    # validate username
    if (!$form_state['values']['username'])
        form_set_error('username', t('Please enter a username.'));
    elseif (user_load_by_name($form_state['values']['username']))
        form_set_error('username', t(
            "The username '!username' is already in use.",
            array(
                '!username' => $form_state['values']['username']
            )
        ));

    # validate email
    if (!$form_state['values']['email'])
        form_set_error('email', t('Please enter your email.'));
    elseif (!valid_email_address($form_state['values']['email']))
        form_set_error('email', t(
            "'!email' does not appear to be a valid email address.",
            array(
                '!email' => $form_state['values']['email']
            )
        ));

    # force acceptance of t&cs
    if (!$form_state['values']['accept_terms'])
        form_set_error('accept_terms', t('Please accept the terms and conditions.'));
 
}

/**
 * Submit handler for registration form
 */
function av_volunteer_registration_form_submit($form, &$form_state) {
    
    if ($user = user_save(
        (object)array(
            'name'   => $form_state['values']['username'],
            'mail'   => $form_state['values']['email'],
            'status' => 0, # pending admin approval
            'roles'  => array(
                2 => 'authenticated user',
                7 => 'Individual Volunteer'
            )
        )
    )) {

        drupal_set_message(t(
            "User '!username' successfully created.",
            array(
                '!username' => $form_state['values']['username']
            )
        ));
        
        civicrm_initialize();

        # get associated contact id for the user
        $result = civicrm_api('UFMatch', 'getsingle', $params = array(
            'version' => 3,
            'uf_id'   => $user->uid
        ));

        if (isset($result['contact_id'])) {
            
            # set contact info
            $contact = civicrm_api('Contact', 'create', array(
                'version' => 3,
                'id'         => $result['contact_id'],
                'first_name' => $form_state['values']['first_name'],
                'last_name'  => $form_state['values']['last_name']
            ));
            
            if ($contact['is_error']) {
                watchdog(
                    'abiltynet_volunteer',
                    t(
                        'An error occurred setting contact info for !username during account generation: !message',
                        array(
                            '!username' => $user->name,
                            '!message'  => $contact['error_message']
                        )
                    ),
                    array(),
                    WATCHDOG_ERROR
                );

            } else {
                # if that went ok, set phone number
                $phone = civicrm_api('Phone', 'create', array(
                    'version'          => 3,
                    'contact_id'       => $result['contact_id'],
                    'location_type_id' => 1, # Home
                    'phone'            => $form_state['values']['mobile'],
                    'is_primary'       => 1,
                    'phone_type_id'    => 2  # Mobile
                ));

                if ($phone['is_error'])
                    watchdog(
                        'abiltynet_volunteer',
                        t(
                            'An error occurred setting phone record for !username during account generation: !message',
                            array(
                                '!username' => $user->name,
                                '!message'  => $phone['error_message']
                            )
                        ),
                        array(),
                        WATCHDOG_ERROR
                    );
            }

        } else {
            watchdog(
                'abiltynet_volunteer', 
                t(
                    'Unable to located associated contact for !username during user account generation',
                    array('!username' => $user->name)
                ), 
                array(),
                WATCHDOG_ERROR
            );
        }

        # send notification email to administrator

        # redirect to thankyou page
        drupal_goto('volunteer/registration/thanks');
    
    } else {
        drupal_set_message(t("An error occurred creating user."), 'error');
    }
    
}
