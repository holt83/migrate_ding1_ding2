<?php
/**
 * @file
 * Class for migrating files into Ding2.
 */

class DingFileMigration extends DrupalFile6Migration {
  public function __construct() {
    // The description and the migration machine name are also required arguments,
    // which will be unique for each migration you register.
    $file_arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'description' => t('Migration of files from Ding1/D6'),
      'machine_name' => 'DingFile',
      'source_dir' => '/home/quickstart/websites/d6.vejlebib.dev/sites/default/files/',
      'user_migration' => 'User',
    );
    
    parent::__construct($file_arguments);
  }
}