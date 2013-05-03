<?php

abstract class DingTableMigration extends Migration {
  public function __construct($arguments) {
    parent::__construct();
    $this->description = 'Migration of ding_place2book table from Ding1/D6';
    
    $this->dependencies = $arguments['dependencies'];
    $this->sourceConnection = 'legacy';
    $this->machine_name = $arguments['machine_name'];
    $this->source_table = $arguments['source_table'];
    $this->destination_table = $arguments['destination_table'];
    $this->source_key = $arguments['source_key'];
    
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select($this->source_table, 't')
             ->fields('t');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTable($this->destination_table);

    $this->map = new MigrateSQLMap(
      $this->machineName, 
      $this->source_key,
      MigrateDestinationTable::getKeySchema()
    );
  }
}

/**
 * Common mappings for table migrations.
 */


class DingPlace2bookMigration extends DingTableMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of ding_place2book table from Ding1/D6'),
      'machine_name' => 'DingPlace2book',
      'source_table' => 'ding_place2book',
      'destination_table' => 'ding_place2book',
      'dependencies' => array('DingEvent'),
      'source_key' => array(
        'nid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source node ID of related event node',
          'alias' => 't',
        )
      ),
    );

    parent::__construct($arguments);

    $this->addSimpleMappings(array('place2book_id', 'capacity', 'maintain_copy', 'kultunaut_export', 'passive'));
    
    $this->addFieldMapping('nid', 'nid')
         ->sourceMigration('DingEvent');
  }
  
  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }

    // If event node doesn't exist, do not process this row
    // Implement this by adding an extra row to the query function, so event table nid is included as legacy_event_nid - then this step will be easy..
    /*
    if (event nid does not exist) {
       return FALSE;
    }
    */
    
    watchdog('migrate_ding1_ding2', 'processing place2book row: old_nid = ' . $row->nid . ' maintain_copy = ' . $row->maintain_copy);
  }

}
