<?php

class Opportunity_Status_Change {

    public function __construct($nid, $old_status, $new_status) {
        watchdog('andyw', 'class constructor');
    }

};