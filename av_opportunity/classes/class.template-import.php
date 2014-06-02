<?php

class Opportunity_Template_Import {

    private $force, $import_dir;

    /**
     * Constructor
     */
    public function __construct() {

        $this->force        = @drush_get_option('force', 0);
        $this->import_dir   = realpath(__DIR__ . '/../message_templates');
        $this->import_files = $this->getFiles();

        $this->run();
        
    }

    /**
     * Get all xml files in import dir
     * @return array
     */
    private function getFiles() { 
        return glob($this->import_dir . '/*.xml');
    }

    /**
     * Import runner - for each file, load, parse, then run importTemplate
     * @return bool
     */
    private function run() {

        # iterate through files in import dir
        foreach ($this->import_files as $file) {
            
            # load the file and parse into array
            if (!$contents = @file_get_contents($file))
                return (bool)drush_log(dt('Unable to load file: @file', array('@file' => $file)), 'error');
            if (!$xml = Opportunity_XML2Array::createArray($contents))
                return (bool)drush_log(dt('Unable to parse xml in @file', array('@file' => $file)), 'error');
            
            # juggle data into the form we want
            $params                  = $xml['template'];
            $params['template_name'] = 'opportunities_' . $params['template_name'];
            $params['text']          = reset($params['text']);
            $params['html']          = reset($params['html']);
            $params['overwrite']     = $this->force; 

            # save template
            $template = new Opportunity_Message_Template($params);
            $success  = $template->save();

            # act on any errors which occurred
            if (!$success) {
                if ($template->isError()) {
                    print_r($template->getErrors());
                    foreach ($template->getErrors() as $error_message)
                        drush_log(dt(
                            "Template '@xmlfile' @error", array(
                                '@xmlfile' => basename($file),
                                '@error'   => strtolower($error_message),
                            )   
                        ), 'error');
                } else {
                    drush_log(dt('Failed to saved record: unknown error'), 'error');
                }
                return false;
            }

            drush_log(dt('Imported template: @xmlfile', array('@xmlfile' => basename($file))), 'success');
        
        }

        return true;

    }

};