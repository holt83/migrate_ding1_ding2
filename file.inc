<?php
/**
 * @file
 * Class for migrating files into Ding2.
 */

class DingFileMigration extends DrupalFile6Migration {
  
  public function __construct($input_arguments) {
    // in settings.php file, add the absolute path your ding1 sites files reside in by including the line:
    // $conf['migrate_ding1_ding2_source_dir'] = '/home/YOUR-DING1-SITE/sites/default/files';
    $source_dir = variable_get('migrate_ding1_ding2_source_dir', '/home/drupalpro/websites/libsite-dumps/randersbibdk-files');
    
    // The description and the migration machine name are also required arguments,
    // which will be unique for each migration you register.
    $file_arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'description' => t('Migration of files from Ding1/D6'),
      'machine_name' => 'DingFile',
      'source_dir' => $source_dir,
      'user_migration' => 'DingUser',
    );
    
    parent::__construct($input_arguments + $file_arguments);
  
    // Removing and redefining existing field mapping
    $this->removeFieldMapping('destination_file');
    $this->addFieldMapping('destination_file', 'filepath')
         ->description(t('filepath is stripped of sites/default/files/ before being copied over'))
         ->callbacks(array($this, 'cleanFilepath'));
    // urlencode option, see https://drupal.org/node/1540106#urlencode
    $this->addFieldMapping('urlencode')
         ->description(t('urlencode set to TRUE has no effect on this migration, but we set it anyway'))
         ->defaultValue(1);

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('path'));
  }
  
  //Function to strip "sites/default/files/" from destination_file
  protected function cleanFilepath($value) {
    $value = str_replace('sites/default/files/', '', $value) ;
    return $value;
  }
  
}