<?php
/**
 * @file
 * Handling specific to a Ding1/D6 source for users
 */

class DingUserMigration extends DrupalUser6Migration {
  
  public function __construct(array $arguments) {
    
    $common_arguments = array(
      'description' => t('Migration of Users from Ding1/D6'),
      'machine_name' => 'DingUser',
      'source_connection' => 'legacy',
      'source_version' => 6,
      'role_migration' => 'DingRole',
    );
    parent::__construct($arguments + $common_arguments);

    // Unmapped source fields
    $this->addUnmigratedSources(array(
      'field_image', 
      'field_image:list', 
      'field_multi_library_ref',
      'field_staff_title', 
      'field_work_area', 
      'field_profile_title',
      'field_profile_body',
      'field_ting_staff_ref',
      'field_ting_staff_ref:ting_ref_type',
      'field_ting_staff_ref:description',
    ));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'group_audience',
      'group_audience:state',
      'group_audience:created',
    ));

  }
}
