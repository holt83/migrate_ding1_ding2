<?php
/**
 * Common mappings for table migrations.
 */
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
    
    $this->source = new MigrateSourceSQL($this->query());
    $this->destination = new MigrateDestinationTable($this->destination_table);

    $this->map = new MigrateSQLMap(
      $this->machineName, 
      $this->source_key,
      MigrateDestinationTable::getKeySchema($this->destination_table)
    );
  }

  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select($this->source_table, 't')
             ->fields('t');
    return $query;
  }
}


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

  protected function query() {
    $query = parent::query();

    // To avoid errors coming from event nodes that does not exist, 
    // we join with the node table adding the condition that node id must not be null
    $query->leftJoin('node', 'n', 't.nid=n.nid');
    $query->isNotNull('n.nid');

    return $query;
  }

}


class DingOpeningHoursMigration extends DingTableMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of opening_hours table from Ding1/D6'),
      'machine_name' => 'DingOpeningHours',
      'source_table' => 'opening_hours',
      'destination_table' => 'opening_hours',
      'dependencies' => array('DingLibrary'),
      'source_key' => array(
        'instance_id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'ID of opening hours instance',
          'alias' => 't',
        )
      ),
    );

    parent::__construct($arguments);

    $this->addSimpleMappings(array('instance_id', 'date', 'start_time', 'end_time', 'notice', 'repeat_rule', 'repeat_end_date', 'original_instance_id', 'customised'));
    
    $this->addFieldMapping('nid', 'nid')
         ->sourceMigration('DingLibrary');

    /*
    $this->addFieldMapping('instance_id')
         ->description(t('Let instance_id count up itself on the destination'));
    $this->addFieldMapping(NULL, 'instance_id')
         ->description(t('Let instance_id count up itself on the destination'));
    // Unmapped source fields
    $this->addUnmigratedSources(array('instance_id'));
    */

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('category_tid'));
    
  }
  
  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Fix repeat end dates that were incorrectly set as 0000-00-00 by inserting today 20 years in the future
    if ($row->repeat_end_date == '0000-00-00') {
      $row->repeat_end_date = date('Y-m-d', strtotime('+20 years'));

      // Logging
      watchdog('migrate_ding1_ding2', 'Fixing repeat_end_date of 0000-00-00 by setting it to future date %date', array('%date'=>$row->repeat_end_date), WATCHDOG_INFO);
    }
  }
  

}
