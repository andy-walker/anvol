<?php

/**
 * Drush script included with the av_opportunity module
 * Currently used to install email templates, but may add more tasks as we go along
 * andyw@circle, 30/05/2014
 */

/**
 * Implementation of hook_drush_command
 */
function av_opportunity_drush_command() {
	
	$items = array();

	$items['av-opportunity'] = array(
		'description' => dt("Perform administration tasks relating to the av_opportunity module"),
		'arguments'   => array(),
		'options'     => array(),
		'examples'    => array('example usage'),
		'bootstrap'   => DRUSH_BOOTSTRAP_DRUPAL_FULL, // Full bootstrap
        'aliases'     => array('avo'),
	);
	
	return $items;

}

/**
 * Implementation of hook_drush_help
 */
function av_opportunity_drush_help($section) {
	
    switch ($section) {
		case 'drush:av-opportunity':
			return dt(
                "Perform administration tasks relating to opportunities - " . 
                "eg: drush abilitynet-opportunity install-templates."
            );
	}

}

/**
 * Command callback
 */
function drush_av_opportunity($operation=null, $arg1=null, $arg2=null) {
	
    civicrm_initialize();

    switch ($operation) {
        
        case 'install-templates': return new Opportunity_Template_Import();
        
        case 'status-change':
            if (!($nid = $arg1) or !($status = $arg2))
                return drush_log(dt("Usage: drush ao status-change <nid> <status>"), 'error');
            if (!$node = node_load($nid))
                return drush_log(dt("Unable to load node (nid @nid)", array('@nid' => $nid)), 'error');
            # todo: check allowed statuses
            # todo: change status
            # todo: save the node
            break;
        
        case null:
            return drush_log(dt('Please specify an operation - eg: drush av-opportunity install-templates'), 'error');
        
        default:
            return drush_log(dt("Unrecognized operation: '@op'", array('@op' => $operation)), 'error');
    }

}