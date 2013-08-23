<?php
/**
 * @file
 * Common mappings for migration into Ding2/D7 Profile2 entities.
 */
abstract class DingProfile2Migration extends DrupalUser6Migration {
  public function __construct(array $arguments) {
    $common_arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'user_migration' => 'DingUser',
    );
    
    parent::__construct($arguments + $common_arguments);

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Ewan Andreasen', 'ea@vejlebib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
    );

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

    // Unmapped source fields common to profile2 migrations
    $this->addUnmigratedSources(array(
      'mode',
      'sort',
      'threshold',
      'uid',
      'mail',
      'name',
      'pass',
      'status',
      'created',
      'access',
      'login',
      'picture',
      'signature',
      'signature_format',
      'timezone',
      'timezone_name',
      'language',
      'theme',
      'init',
      'data',
      'field_profile_title',
      'field_profile_body',
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
      'path',
    ));

    // Common unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'uid',
      'revision_uid',
      'path',
      'access',
      'created',
      'data',
      'init',
      'language',
      'login',
      'mail',
      'name',
      'pass',
      'pathauto',
      'picture',
      'roles',
      'signature',
      'signature_format',
      'status',
      'theme',
      'timezone',
    ));
  }

}


class DingProfileProviderAlmaMigration extends DingProfile2Migration {
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of user provider data from Ding1/D6'),
      'machine_name' => 'DingProfileProviderAlma',
      'group_name' => 'ding1_group',
      'dependencies' => array('DingUser', 'DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['interest_period'] = t('Interest period extracted from field data');

    parent::__construct($arguments);

    $this->destination = new MigrateDestinationProfile2('provider_alma');

    //map field_alma_interest_period
    $this->addFieldMapping('field_alma_interest_period', 'interest_period');

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

  // Modify the query to include content profile data
  protected function query() {
    $query = parent::query();
    //$query->leftJoin('ding_campaign', 'dc', 'n.nid=dc.nid AND n.vid = dc.vid');
    //$query->addField('dc', 'campaign_weight');
    //$query->addField('dc', 'campaign_type');
    return $query;
  }
}
