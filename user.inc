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
      'group_name' => 'ding1_group',
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
      'field_image:alt', 
      'field_multi_library_ref',
      'field_staff_title', 
      'field_work_area', 
      'field_firstname', 
      'field_phone', 
      'field_surname', 
      '2',
      'field_profile_title',
      'field_profile_body',
    ));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'group_audience',
      'group_audience:state',
      'group_audience:created',
    ));

  }
}
