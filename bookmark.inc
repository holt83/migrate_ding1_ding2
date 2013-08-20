<?php
/**
 * Common mappings for flag migrations.
 */
abstract class DingFlagMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct();
    $this->description = 'Migration of tables from Ding1/D6 into D7 flags';
    
    $this->dependencies = $arguments['dependencies'];
    $this->sourceConnection = 'legacy';
    $this->machine_name = $arguments['machine_name'];
    $this->source_table = $arguments['source_table'];
    $this->destination_fid = $arguments['destination_fid'];
    $this->source_key = $arguments['source_key'];
    
    $this->source = new MigrateSourceSQL($this->query());
    $this->destination = new MigrateDestinationFlagSimple($this->destination_fid);
    
    $this->map = new MigrateSQLMap(
      $this->machineName, 
      $this->source_key,
      MigrateDestinationFlagSimple::getKeySchema()
    );
  }

  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select($this->source_table, 't')
             ->fields('t');
    return $query;
  }
}


class DingBookmarkMigration extends DingFlagMigration {
  public function __construct() {
    // "bookmark" flags in ding2 has fid = 2
    $bookmark_fid = 2;
    
    $arguments = array(
      'description' => t('Migration of ding_cart table from Ding1/D6 into D7 bookmark flags (ding_bookmark)'),
      'machine_name' => 'DingBookmark',
      'source_table' => 'ding_cart',
      'destination_fid' => $bookmark_fid,
      'dependencies' => array('DingUser'),
      'source_key' => array(
        'uid' => array(
          'description' => t('{users}.uid for user'),
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
        'local_id' => array(
          'description' => t('Local ID in Ting lingo.'),
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),  
      ),
    );
    
    parent::__construct($arguments);

    $this->addFieldMapping('uid', 'uid')
         ->sourceMigration('DingUser');

    $this->addFieldMapping('fid')
         ->defaultValue($bookmark_fid)
         ->description(t('hardcoded to 2, the assigned value of bookmark flags in ding2'));
    $this->addFieldMapping('content_type')
         ->defaultValue('ding_entity');
    $this->addFieldMapping('content_id', 'ting_id')
         ->description(t('Handled in prepareRow function'));
    $this->addFieldMapping('timestamp', 'created');
    
    // Unmapped source fields
    $this->addUnmigratedSources(array('title', 'author', 'type', 'thumbnail_url'));
    
    // Unmapped destination fields
    //$this->addUnmigratedDestinations(array('category_tid'));
    
  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // we populate the ting_object table during migrating and replace ting_id with tid from ting_object_table
    $row->ting_id = migrate_ding1_ding2_add_ting_object($row->ting_id);
  }

}
