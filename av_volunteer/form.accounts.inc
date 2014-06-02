<?php

function av_volunteer_accounts($form, $form_state) {

    return $form + array(
        
        'charity_name' => array(
            '#type'  => 'textfield',
            '#title' => t('Charity Name'),
            '#size'  => 70
        ),

        'address' => array(
            '#type'  => 'textfield',
            '#title' => t('Address'),
            '#size'  => 70
        ),

        'postcode' => array(
            '#type'  => 'textfield',
            '#title' => t('Postcode'),
            '#size'  => 31
        ),

        'phone_number' => array(
            '#type'  => 'textfield',
            '#title' => t('Phone Number'),
            '#size'  => 31
        ),

        'mobile_number' => array(
            '#type'  => 'textfield',
            '#title' => t('Mobile Number'),
            '#size'  => 31
        ),

        'email' => array(
            '#type'  => 'textfield',
            '#title' => t('Email'),
            '#size'  => 70
        ),

        'account_manager' => array(
            '#type'   => 'item',
            '#title'  => t('Account Manager'),
            '#markup' => l(t('circleinteractive'), 'user/1')
        ),

        'group_name' => array(
            '#type'        => 'fieldset',
            '#title'       => t('Group Name'),
            '#collapsible' => true,
            '#collapsed'   => true
        ),

        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        )

    );

}

function av_volunteer_accounts_details_tab($form, $form_state) {

    return $form + array(
        
        'description' => array(
            '#type'  => 'textarea',
            '#title' => t('Charity or Region Description')
        ),

        'registration_no' => array(
            '#type'  => 'textfield',
            '#title' => t('Registration No'),
            '#size'  => 31
        ),       
        
        'submit' => array(
            '#type'  => 'submit',
            '#value' => t('Save')
        )
        
    );

}

function av_volunteer_accounts_members_tab($form, $form_state) {

}

function av_volunteer_accounts_opportunities_tab($form, $form_state) {

}

function av_volunteer_accounts_password_tab($form, $form_state) {

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
