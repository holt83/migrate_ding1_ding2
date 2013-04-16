<?php
/**
 * @file
 * Handling specific to a Ding1/D6 source for users
 */
class DingUserMigration extends DrupalUser6Migration {
  public function __construct(array $arguments) {
    $arguments = array(
      'description' => t('Migration of Users from Ding1/D6'),
      'machine_name' => 'DingUser',
      'source_connection' => 'legacy',
      'source_version' => 6,
      'role_migration' => 'DingRole',
    );
    
    parent::__construct($arguments);

    // Unmapped source fields
    $this->addUnmigratedSources(array(
      'field_department', 
      'field_image', 
      'field_image:list', 
      'field_image:data', 
      'field_multi_library_ref',
      'field_staff_title', 
      'field_work_area', 
      'field_firstname', 
      'field_phone', 
      'field_surname', 
      '2', 
    ));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'group_audience',
    ));

  }
}
