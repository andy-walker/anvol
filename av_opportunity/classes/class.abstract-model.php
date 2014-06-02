<?php

/**
 * Generic data model class
 * andyw@circle, 01/06/2014
 */
abstract class Opportunity_Abstract_Model {
    
    /**
     * Internal error stack
     * @var array
     */
    protected $errors;

    /**
     * Constructor
     * @param $properties (array) (optional)
     */
    public function __construct($properties = array()) {
        $this->errors = array();
        $this->set($properties);
    }

    /**
     * Force child to implement load method to set internal properties
     * @return success (bool)
     * @abstract
     */
    abstract public function load();
    
    /**
     * Force child to implement a save method to save internal properties to db
     * @return success (bool)
     * @abstract
     */
    abstract public function save();

    /**
     * Force child to implement a validate method to validate internal properties before saving to db
     * Should push errors onto the error stack if any occur
     * @return bool
     * @abstract
     */
    abstract protected function validate();

    /**
     * Clear error message stack - should be called before performing a new load or save operation
     */
    protected function clearErrors() {
        $this->errors = array();
    }

    /**
     * Set error
     * @param $msg (string)
     */
    protected function error($message, $arguments = array()) {
        # alias correct t() function, depending on environment
        $t = function_exists('dt') ? 'dt' : 't';
        $this->errors[] = $t($message, $arguments);
    }

    /**
     * Get errors
     * @return array
     */
    public function getErrors() {
        return $this->errors;
    }

    /**
     * Check object error state
     * @return bool
     */
    public function isError() {
        return (bool)count($this->errors);
    }

    /**
     * Internal setter function
     * @param $properties (array) - associative array of key / value pairs corresponding to model properties and their values
     */
    protected function set($properties) {
        foreach ($properties as $property => $value)
            $this->$property = $value;
    }


}