<?php
/**
 * @file
 * Classes for migrating into Ding2/D7 Profile2 entities.
 */

class DingProfileProviderAlmaMigration extends Migration {
  
  public function __construct($arguments) {
    
    global $user;
    
    $this->team = array(
      new MigrateTeamMember('Ewan Andreasen', 'ea@vejlebib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
    );
    $this->description = t('Migration of user provider data from Ding1/D6');
    $this->dependencies = array('DingUser', 'DingFile');

    $arguments = array(
      'machine_name' => 'DingProfileProviderAlma',
      'group_name' => 'ding1_group',
    );

    parent::__construct($arguments);

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'uid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationProfile2::getKeySchema()
    );
    
    // Provide our own description of the interest_period field from the source.
    $source_fields = array(
      'interest_period' => t('Interest period extracted from field data')
    );
    
    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($this->query(), $source_fields, NULL);

    $this->destination = new MigrateDestinationProfile2('provider_alma');

    //map fields
    $this->addFieldMapping('uid', 'uid')
         ->sourceMigration('DingUser')
         ->description(t('The assignment of profile2-items to the respective user'));
    $this->addFieldMapping('revision_uid')
      ->defaultValue($user->uid)
      ->description(t('The user ID of the user, who started the migration'));
    $this->addFieldMapping('language')
      ->defaultValue('und');
    $this->addFieldMapping('field_alma_interest_period', 'interest_period');
    $this->addFieldMapping(NULL, 'data')
         ->description(t('Handled in prepareRow function'));
    
    // Unmapped destination fields
    $this->addFieldMapping('field_alma_absent_id')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_absent_id:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_city')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_city:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_full_name')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_full_name:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_preferred_branch')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_mail')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_mobile_phone')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_mobile_phone:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_phone_id')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_phone_id:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_postal_code')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_postal_code:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_reservation_pause')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_reservation_pause:value2')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_street_name')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_alma_street_name:language')
      ->description(t('Not mapped - User data displayed directly through Alma Provider'))
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('path')
      ->issueGroup(t('DNM'));
  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }

    // Get values from serialized data and populate the source fields defined in our constructor
    $user_data = unserialize($row->data);
    $row->interest_period = $user_data['interest_period'];
  }

  // Modify the query to select only alma users
  protected function query() {
    $query = Database::getConnection('default', 'legacy')
             ->select('users', 'u')
             ->fields('u', array('uid', 'data'));
    $query->condition('name','%@alma_user', 'LIKE');
    return $query;
  }
  
}


class DingProfileStaffMigration extends Migration {
  public function __construct($arguments) {
    global $user;
    
    $this->team = array(
      new MigrateTeamMember('Ewan Andreasen', 'ea@vejlebib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
      new MigrateTeamMember('Simon Holt', 'sh@randersbib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
    );
    $this->description = t('Migration of user content profile data from Ding1/D6');
    $this->dependencies = array('DingUser', 'DingFile', 'DingLibrary');

    $arguments = array(
      'machine_name' => 'DingProfileStaff',
      'group_name' => 'ding1_group',
    );
    
    parent::__construct($arguments);

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'nid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'D6 Unique Node ID', 
          'alias' => 'n',
        )
      ),
      MigrateDestinationProfile2::getKeySchema()
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($this->query());
    
    $this->destination = new MigrateDestinationProfile2('ding_staff_profile');

    //map fields
    $this->addFieldMapping('uid', 'uid')
         ->sourceMigration('DingUser')
         ->description(t('The assignment of profile2-items to the respective user'));
    $this->addFieldMapping('revision_uid')
      ->defaultValue($user->uid)
      ->description(t('The user ID of the user, who started the migration'));
    $this->addFieldMapping('language')
      ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_description', 'body');
    $this->addFieldMapping('field_ding_staff_description:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_staff_description:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_position', 'field_staff_title_value');
    $this->addFieldMapping('field_ding_staff_position:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_forename', 'field_firstname_value');
    $this->addFieldMapping('field_ding_staff_forename:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_surname', 'field_surname_value');
    $this->addFieldMapping('field_ding_staff_surname:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_phone', 'field_phone_value');
    $this->addFieldMapping('field_ding_staff_phone:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_staff_email', 'mail');
    

    // Unmapped source fields
    $this->addUnmigratedSources(array(
          'teaser',
          'title',
          'field_work_area_value',
          'nr_title',
          'name',
        ));

    // Unmapped destination fields    
    $this->addUnmigratedDestinations(array(
      'path',
      'field_ding_staff_image',
      'field_ding_staff_image:file_class',
      'field_ding_staff_image:language',
      'field_ding_staff_image:preserve_files',
      'field_ding_staff_image:destination_dir',
      'field_ding_staff_image:destination_file',
      'field_ding_staff_image:file_replace',
      'field_ding_staff_image:source_dir',
      'field_ding_staff_image:urlencode',
      'field_ding_staff_image:alt',
      'field_ding_staff_image:title',
      'field_ding_staff_department',
      'field_ding_staff_department:source_type',
      'field_ding_staff_department:create_term',
      'field_ding_staff_department:ignore_case',
      'field_ding_staff_library',
      'field_ding_staff_library:state',
      'field_ding_staff_library:created',
    ));
  }

  // Modify the query to include content profile data
  protected function query() {
    $query = Database::getConnection('default', 'legacy')
          ->select('node', 'n')
          ->fields('n', array('nid', 'title'))
          ->condition('n.type', 'profile', '='); 
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid'); 
    $query->addField('nr', 'body'); 
    $query->addField('nr', 'teaser'); 
    $query->addField('nr', 'title'); 
    $query->join('users', 'u', 'n.uid = u.uid'); 
    $query->addField('u', 'uid');
    $query->addField('u', 'name'); 
    $query->addField('u', 'mail'); 
    $query->leftJoin('content_type_profile', 'ctp', 'n.nid = ctp.nid AND n.vid = ctp.vid'); 
    $query->addField('ctp','field_staff_title_value'); 
    $query->addField('ctp','field_work_area_value'); 
    $query->addField('ctp','field_firstname_value'); 
    $query->addField('ctp','field_phone_value'); 
    $query->addField('ctp','field_surname_value'); 
    $query->orderBy('n.nid', 'ASC'); 
    // See more at: http://www.grasmash.com/article/migrate-classes-content-profile-profile2#sthash.5JUMSwgz.dpuf
    return $query;
  }
}
