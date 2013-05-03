<?php
/**
 * @file
 * Migration of Ding1/D6 source user fields into Profile2 entities.
 */
class DingProfile2Migration extends DrupalUser6Migration {
  public function __construct(array $arguments) {
    $arguments = array(
      'description' => t('Migration of User fields from Ding1/D6 into profile2 entities'),
      'machine_name' => 'DingProfile2',
      'source_connection' => 'legacy',
      'source_version' => 6,
      //'role_migration' => 'DingRole',
    );
    
    parent::__construct($arguments);

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationProfile2::getKeySchema()
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($this->query(), $this->sourceFields, NULL, $this->sourceOptions);
    $this->destination = new MigrateDestinationProfile2('provider_alma');

    $this->addFieldMapping('uid')
         ->defaultValue(1);
    $this->addFieldMapping('field_alma_full_name')
         ->defaultValue('name-comes-from-alma');
    $this->addFieldMapping('field_alma_full_name:language')
         ->defaultValue('und');
    
         
    /*
    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('id'));
    */
  }

}
