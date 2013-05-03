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
      MigrateDestinationTable::getKeySchema()
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
