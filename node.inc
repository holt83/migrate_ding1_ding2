<?php

/**
 * Common mappings for the Drupal 6 node migrations.
 */
abstract class DingNodeMigration extends DrupalNode6Migration {
  public function __construct(array $input_arguments) {
    $arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'user_migration' => 'DingUser',
    );
    
    parent::__construct($input_arguments + $arguments);

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Ewan Andreasen', 'ea@vejlebib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
      new MigrateTeamMember('Simon Holt', 'sh@randersbib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
    );

    $this->addFieldMapping('field_legacy_nid', 'nid')
         ->description('We have a common field to save the D6 nid');
         
    // Unmapped source fields - EXAMPLE
    //$this->addUnmigratedSources(array('vid', 'tnid', 'translate', 'teaser', 'format'));
    // Unmapped destination fields - EXAMPLE
    //$this->addUnmigratedDestinations(array('log', 'tnid', 'translate', 'body:summary', 'body:format', 'body:language', 'comment'))
  }
  
  // function to create ting objects with relations to the node
  public function createDingNodeTingRefs($node, $row) {
    
    if (isset($row->field_ting_refs) && count($row->field_ting_refs) > 0) {

      $refs = '';

      foreach ($row->field_ting_refs as $ref) {
        if ($ref) {
          // Save a ting object entity
          $tid = migrate_ding1_ding2_add_ting_object($ref);

          // Create relation
          if ($tid) {
            $rel_ent = ting_reference_create_relation('node', $node->nid, $tid);

            // Append to logging string
            $refs .= $ref . ', ';
          }
          
        }
      }

      $refs = rtrim($refs, ', '); 

      // Logging
      if ($refs != '') {
        watchdog('migrate_ding1_ding2', 'Created Ting object reference(s) (%refs) with relation(s) on item (%title - D7 nid %d7nid)', array('%refs' => $refs, '%title'=>$node->title, '%d7nid'=>$node->nid), WATCHDOG_INFO);
      }
    }
  }
  
}


class DingLibraryMigration extends DingNodeMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Libraries from Ding1/D6'),
      'machine_name' => 'DingLibrary',
      'source_type' => 'library',
      'destination_type' => 'ding_library',
      'dependencies' => array('DingUser','DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    $this->sourceFields['list_image_alt'] = t('Alt value extracted from field_list_image_data');
    $this->sourceFields['image_title'] = t('Title text extracted from field_image_data');
    $this->sourceFields['image_alt'] = t('Alt value extracted from field_image_data');
    
    parent::__construct($input_arguments + $arguments);
    
    $this->removeFieldMapping('body');
    $this->addFieldMapping('field_ding_library_body', 'body');
    $this->addFieldMapping('field_ding_library_body:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_body:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_library_lead', 'teaser');
    $this->addFieldMapping('field_ding_library_lead:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_mail', 'field_email');
    $this->addFieldMapping('group_group')
         ->defaultValue('1');

    // Phone and fax comes from location_phone table and is drawn in via the modified query
    $this->addFieldMapping('field_ding_library_phone_number', 'phone'); 
    $this->addFieldMapping('field_ding_library_phone_number:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_fax_number', 'fax'); 
    $this->addFieldMapping('field_ding_library_fax_number:language')
         ->defaultValue('und');

    // library slug comes from ding_library table and is drawn in via the modified query
    $this->addFieldMapping('field_ding_library_slug', 'slug'); 
    $this->addFieldMapping('field_ding_library_slug:language')
         ->defaultValue('und');

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_library_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_library_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_library_list_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_list_image:preserve_files')
         ->defaultValue(FALSE);
    $this->addFieldMapping('field_ding_library_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_library_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_list_image:alt')
         ->issueGroup(t('DNM'));         
    
    // Map the image field to the new title image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_library_title_image', 'field_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_library_title_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_library_title_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_title_image:preserve_files')
         ->defaultValue(FALSE);
    $this->addFieldMapping('field_ding_library_title_image:alt', 'image_alt');
    $this->addFieldMapping('field_ding_library_title_image:title', 'image_title');
    $this->addFieldMapping(NULL, 'field_image:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_image:alt')
         ->issueGroup(t('DNM'));         
    
    // Map to addressfield. See https://drupal.org/node/1996546.
    $this->addFieldMapping('field_ding_library_addresse', 'country');
    $this->addFieldMapping('field_ding_library_addresse:thoroughfare', 'street');
    $this->addFieldMapping('field_ding_library_addresse:premise', 'additional');
    $this->addFieldMapping('field_ding_library_addresse:locality', 'city');
    $this->addFieldMapping('field_ding_library_addresse:administrative_area', 'province');
    $this->addFieldMapping('field_ding_library_addresse:postal_code', 'postal_code');
    $this->addFieldMapping('field_ding_library_addresse:name_line', 'name');
    
    // Map to geofield.
    $this->addFieldMapping('field_ding_library_geocode')
         ->defaultValue('point');
    $this->addFieldMapping('field_ding_library_geocode:geo_type')
         ->defaultValue('point');
    $this->addFieldMapping('field_ding_library_geocode:lat', 'latitude');
    $this->addFieldMapping('field_ding_library_geocode:top', 'latitude');
    $this->addFieldMapping('field_ding_library_geocode:bottom', 'latitude');
    $this->addFieldMapping('field_ding_library_geocode:lon', 'longitude');
    $this->addFieldMapping('field_ding_library_geocode:left', 'longitude');
    $this->addFieldMapping('field_ding_library_geocode:right', 'longitude');

    // The opening_hours field in Ding2/D7 will be handled by migrating the opening_hours table of the module opening_hours
    $this->addFieldMapping('field_ding_library_opening_hours')
         ->description(t('Initially empty strings - field is populated by use of the site'))
         ->defaultValue('');
    $this->addFieldMapping('field_ding_library_opening_hours:language')
         ->defaultValue('und');

    // library links
    $this->addFieldMapping('field_ding_library_links', 'field_links');
    $this->addFieldMapping('field_ding_library_links:title', 'field_links:title');
    $this->addFieldMapping('field_ding_library_links:attributes', 'field_links:attributes');
    $this->addFieldMapping('field_ding_library_links:language')
         ->defaultValue('und');

    // Unmapped source fields
    $this->addFieldMapping(NULL, 'field_content_images')
         ->description(t('content images are not mapped to Ding2/D7'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images:list')
         ->description(t('content images are not mapped to Ding2/D7'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images:alt')
         ->description(t('content images are not mapped to Ding2/D7'))
         ->issueGroup(t('DNM'));         
    $this->addFieldMapping(NULL, 'field_pc_booking_method')
         ->description(t('Ding2/D7 does not employ pc booking'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_pc_booking_url')
         ->description(t('Ding2/D7 does not employ pc booking'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_feature_ref')
         ->description(t('Ding2/D7 does not employ feature nodes'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:week_start')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:week_end')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:sun_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:sun_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:mon_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:mon_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:tue_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:tue_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:wed_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:wed_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:thu_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:thu_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:fri_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:fri_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:sat_open')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_opening_hours:sat_close')
         ->description(t('Ding2/D7 uses the opening_hours module instead of office_hours module. Handled by separate migration of the opening_hours table'))
         ->issueGroup(t('DNM'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'field_ding_library_addresse:sub_administrative_area',
      'field_ding_library_addresse:dependent_locality',
      'field_ding_library_addresse:sub_premise',
      'field_ding_library_addresse:organisation_name',
      'field_ding_library_addresse:first_name',
      'field_ding_library_addresse:last_name',
      'field_ding_library_addresse:data',
      'field_ding_library_geocode:srid',
      'field_ding_library_geocode:accuracy',
      'field_ding_library_geocode:source',
    ));
  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Manually fetch the data subfield from the image fields and populate the
    // the image alt and title fields defined in our constructor.
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_field_list_image', 'cfli')
             ->fields('cfli', array('field_list_image_data'))
             ->condition('cfli.nid', $row->nid)
             ->condition('cfli.vid', $row->vid);
    $list_image_data = unserialize($query->execute()->fetchField());
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_field_image', 'cfi')
             ->fields('cfi', array('field_image_data'))
             ->condition('cfi.nid', $row->nid)
             ->condition('cfi.vid', $row->vid);
    $image_data = unserialize($query->execute()->fetchField());
    
    // Populate our fields defined in the constructor from the data in the 
    // fetched subfields.
    if ($list_image_data) {
      $row->list_image_alt = $list_image_data['alt'];
      $row->list_image_title = $list_image_data['title'];      
    }
    if ($image_data) {
      $row->image_alt = $image_data['alt'];
      $row->image_title = $image_data['title'];
    }

    // Make country code uppercase
    $row->country = strtoupper($row->country);
  }

  // Modify the query to include location data
  // For more info, see:
  // http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture as well as
  // http://www.grasmash.com/article/migrate-classes-location-cck-address-field
  protected function query() {
    $query = parent::query();
    $query->leftJoin('location_instance', 'li', 'n.nid=li.nid AND n.vid = li.vid');
    $query->leftJoin('location', 'l', 'li.lid=l.lid');
    $query->addField('l', 'name');
    $query->addField('l', 'street');
    $query->addField('l', 'additional');
    $query->addField('l', 'city');
    $query->addField('l', 'province');
    $query->addField('l', 'postal_code');
    $query->addField('l', 'country');
    $query->addField('l', 'latitude');
    $query->addField('l', 'longitude');
    $query->leftJoin('location_phone', 'lp', 'lp.lid=l.lid');
    $query->addField('lp', 'phone');
    $query->leftJoin('location_fax', 'lf', 'lf.lid=l.lid');
    $query->addField('lf', 'fax');
    $query->leftJoin('ding_library', 'dl', 'n.nid=dl.nid AND n.vid = dl.vid');
    $query->addField('dl', 'slug');
    return $query;
  }
}


class DingCampaignMigration extends DingNodeMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Campaigns from Ding1/D6'),
      'machine_name' => 'DingCampaign',
      'source_type' => 'campaign',
      'destination_type' => 'ding_campaign',
      'dependencies' => array('DingUser', 'DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['campaign_image_alt'] = t('Alt text extracted from field_campaign_image_data');
    $this->sourceFields['campaign_image_title'] = t('Title text extracted from field_campaign_image_data');
    
    parent::__construct($input_arguments + $arguments);

    $this->addFieldMapping('field_camp_text_full', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_text_full:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_full:format')
         ->defaultValue('ding_wysiwyg');

    $this->addFieldMapping('field_camp_text_plain', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_text_plain:language')
         ->defaultValue('und');         

    $this->addFieldMapping('field_camp_link', 'field_campaign_link');
    $this->addFieldMapping('field_camp_link:language')
         ->defaultValue('und');    
    $this->addFieldMapping(NULL, 'field_campaign_link:title')
         ->description(t('Ding2 does not have title data on the campaign links'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_campaign_link:attributes')
         ->description(t('Ding2 does not have attribute data on the campaign links'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping('field_camp_weight', 'campaign_weight');
    $this->addFieldMapping('field_camp_settings', 'campaign_type')
         ->description(t('Handled in prepareRow of DingCampaignMigration'));
    $this->addFieldMapping(NULL, 'field_campaign_image:list')
         ->issueGroup(t('DNM'));
         
    $this->addFieldMapping('field_camp_image:alt', 'campaign_image_alt');
    $this->addFieldMapping('field_camp_image:title', 'campaign_image_title');           
    $this->addFieldMapping('field_camp_image', 'field_campaign_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_camp_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_camp_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_image:preserve_files')
         ->defaultValue(FALSE);
         
    // Unmapped source fields
    $this->addFieldMapping(NULL, 'revision')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
  }
  
  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Manually fetch the data subfield from the image fields and populate the
    // the image alt and title fields defined in our constructor.
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_type_campaign', 'ctc')
             ->fields('ctc', array('field_campaign_image_data'))
             ->condition('ctc.nid', $row->nid)
             ->condition('ctc.vid', $row->vid);
    $campaign_image_data = unserialize($query->execute()->fetchField());    
    
    // Populate our fields defined in the constructor from the data in the 
    // fetched subfields.
    if ($campaign_image_data) {
      $row->campaign_image_alt = $campaign_image_data['alt'];
      $row->campaign_image_title = $campaign_image_data['title'];      
    }
    
    // Modify text in campaign_type
    $row->campaign_type = str_replace('-only', '', $row->campaign_type);
  }
  
  // Modify the query to include ding_campaign data
  protected function query() {
    $query = parent::query();
    $query->leftJoin('ding_campaign', 'dc', 'n.nid=dc.nid AND n.vid = dc.vid');
    $query->addField('dc', 'campaign_weight');
    $query->addField('dc', 'campaign_type');
    return $query;
  }
}


class DingPageMigration extends DingNodeMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Pages from Ding1/D6'),
      'machine_name' => 'DingPage',
      'source_type' => 'page',
      'destination_type' => 'ding_page',
      'dependencies' => array('DingUser','DingTagsTerm', 'DingLibrary', 'DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    $this->sourceFields['list_image_alt'] = t('Alt value extracted from field_list_image_data');
    
    parent::__construct($input_arguments + $arguments);

    $this->addFieldMapping('field_ding_page_body', 'body');
    $this->addFieldMapping('field_ding_page_body:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_page_body:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_page_lead', 'field_teaser');
    $this->addFieldMapping('field_ding_page_lead:language')
         ->defaultValue('und');

    // Files mapping
    $this->addFieldMapping('field_ding_page_files', 'field_file_attachments')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_page_files:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_page_files:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_page_files:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining page file fields
    $this->addFieldMapping('field_ding_page_files:description', 'field_file_attachments:description');
    $this->addFieldMapping('field_ding_page_files:display', 'field_file_attachments:description')
         ->description(t('Use file_attachments_description again for this value'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_page_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_page_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_page_list_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_page_list_image:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_page_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_page_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // ting references
    $this->addFieldMapping(NULL, 'field_ting_refs')
         ->description(t('Handled in complete function'));
    $this->addFieldMapping(NULL, 'field_ting_refs:ting_ref_type')
         ->description(t('Not mapped - Ding2 does not employ ref types'));
    $this->addFieldMapping(NULL, 'field_ting_refs:description')
         ->description(t('Not mapped - Ding2 does not employ descriptions on ting refs.'));

    // Tags term mapping
    $this->addFieldMapping('field_ding_page_tags')
         ->issueGroup(t('DNM'))
         ->description(t('Not mapped - Tags term does not apply to the page type in Ding1'));
    $this->addFieldMapping('field_ding_page_tags:source_type')
         ->issueGroup(t('DNM'))
         ->description(t('Not mapped - Tags term does not apply to the page type in Ding1'));
    $this->addFieldMapping('field_ding_page_tags:create_term')
         ->issueGroup(t('DNM'))
         ->description(t('Not mapped - Tags term does not apply to the page type in Ding1'));
    $this->addFieldMapping('field_ding_page_tags:ignore_case')
         ->issueGroup(t('DNM'))
         ->description(t('Not mapped - Tags term does not apply to the page type in Ding1'));

    // Unmapped source fields
    $this->addFieldMapping(NULL, 'revision')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_file_attachments:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_library_ref')
         ->description(t('Not mapped - Ding2 does not relate pages to libraries'))
         ->issueGroup(t('DNM'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'field_ding_page_title_image', 
      'field_ding_page_title_image:file_class', 
      'field_ding_page_title_image:language', 
      'field_ding_page_title_image:alt', 
      'field_ding_page_title_image:title',
      'field_ding_page_title_image:destination_dir', 
      'field_ding_page_title_image:destination_file', 
      'field_ding_page_title_image:file_replace', 
      'field_ding_page_title_image:preserve_files', 
      'field_ding_page_title_image:source_dir',
      'field_ding_page_title_image:urlencode',
      'field_ding_page_materials',
    ));
    
  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    // Manually fetch the data subfield from the image fields and populate the
    // the image alt and title fields defined in our constructor.
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_field_list_image', 'cfli')
             ->fields('cfli', array('field_list_image_data'))
             ->condition('cfli.nid', $row->nid)
             ->condition('cfli.vid', $row->vid);
    $list_image_data = unserialize($query->execute()->fetchField());
    
    // Populate our fields defined in the constructor from the data in the 
    // fetched subfields. Since we dont have a delta in the table, we know 
    // there is only one row and we just use an if-statement.
    if ($list_image_data) {
      $row->list_image_alt = $list_image_data['alt'];
      $row->list_image_title = $list_image_data['title'];      
    }
  }

  public function complete($node, stdClass $row) {
    // Create Ting objects and relation to it for each item i field_ting_refs
    // We perform this in complete() function since we need the nid on the news node just created
    parent::createDingNodeTingRefs($node, $row);
  }
  
}


class DingNewsMigration extends DingNodeMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Blogposts/News from Ding1/D6'),
      'machine_name' => 'DingNews',
      'source_type' => 'article',
      'destination_type' => 'ding_news',
      'dependencies' => array('DingUser','DingNewsCategoryTerm','DingTagsTerm','DingFile','DingLibrary'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    $this->sourceFields['list_image_alt'] = t('Alt value extracted from field_list_image_data');

    parent::__construct($input_arguments + $arguments);

    $this->addFieldMapping('field_ding_news_body', 'body');
    $this->addFieldMapping('field_ding_news_body:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_news_body:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_news_lead', 'field_teaser');
    $this->addFieldMapping('field_ding_news_lead:language')
         ->defaultValue('und');
    $this->addFieldMapping(NULL, 'field_library_ref')
         ->description(t('Handled in prepare function'));

    // Files mapping
    $this->addFieldMapping('field_ding_news_files', 'field_file_attachments')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_news_files:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_news_files:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_news_files:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining news file fields
    $this->addFieldMapping('field_ding_news_files:description', 'field_file_attachments:description');
    $this->addFieldMapping('field_ding_news_files:display', 'field_file_attachments:description')
         ->description(t('Use file_attachments_description again for this value'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_news_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_news_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_news_list_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_news_list_image:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_news_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_news_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:list')
         ->issueGroup(t('DNM'));

    // ting references
    $this->addFieldMapping(NULL, 'field_ting_refs')
         ->description(t('Handled in complete function'));
    $this->addFieldMapping(NULL, 'field_ting_refs:ting_ref_type')
         ->description(t('Not mapped - Ding2 does not employ ref types'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_ting_refs:description')
         ->description(t('Not mapped - Ding2 does not employ descriptions on ting refs.'))
         ->issueGroup(t('DNM'));

    // News Category term mapping
    $this->addFieldMapping('field_ding_news_category', '1')
         ->sourceMigration('DingNewsCategoryTerm');
    $this->addFieldMapping('field_ding_news_category:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_news_category:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_news_category:ignore_case')
         ->defaultValue(TRUE);

    // News Tags term mapping
    $this->addFieldMapping('field_ding_news_tags', '2')
         ->sourceMigration('DingTagsTerm');
    $this->addFieldMapping('field_ding_news_tags:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_news_tags:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_news_tags:ignore_case')
         ->defaultValue(TRUE);

    // Unmapped source fields
    $this->addFieldMapping(NULL, 'revision')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_file_attachments:list')
         ->issueGroup(t('DNM'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'field_ding_news_title_image', 
      'field_ding_news_title_image:file_class', 
      'field_ding_news_title_image:language', 
      'field_ding_news_title_image:alt', 
      'field_ding_news_title_image:title',
      'field_ding_news_title_image:destination_dir', 
      'field_ding_news_title_image:destination_file', 
      'field_ding_news_title_image:file_replace', 
      'field_ding_news_title_image:preserve_files', 
      'field_ding_news_title_image:source_dir',
      'field_ding_news_title_image:urlencode',
      'field_ding_news_library',
      'field_ding_news_materials',
      'field_ding_news_library:state',
      'field_ding_news_library:created',
    ));

  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Manually fetch the data subfield from the image fields and populate the
    // the image alt and title fields defined in our constructor.
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_field_list_image', 'cfli')
             ->fields('cfli', array('field_list_image_data'))
             ->condition('cfli.nid', $row->nid)
             ->condition('cfli.vid', $row->vid);
    $list_image_data = unserialize($query->execute()->fetchField());
    
    // Populate our fields defined in the constructor from the data in the 
    // fetched subfields. Since we dont have a delta in the table, we can treat
    // it as one row.
    if ($list_image_data) {
      $row->list_image_alt = $list_image_data['alt'];
      $row->list_image_title = $list_image_data['title'];      
    }
  }

  public function prepare($node, stdClass $row) {
    // Assign news to library by manipulating node object directly. 
    // Ugly, but needs to be done this way until we have bumped OG version to 2.1, which contains the migrate classes needed by OG
    $this->setDingNewsGid($node, $row);
  }

  public function complete($node, stdClass $row) {
    // Create Ting objects and relation to it for each item i field_ting_refs
    // We perform this in complete() function since we need the nid on the news node just created
    parent::createDingNodeTingRefs($node, $row);
  }

  // function to set GID for a ding_news node
  private function setDingNewsGid($node, $row) {
    $old_nid = $row->field_library_ref[0];

    // Process if old Ding1 node has a field_library_ref value
    if ($old_nid) {
      // Get new entity ID for the library from the mappings
      $ding2libnid = db_query("SELECT destid1 FROM {migrate_map_dinglibrary} WHERE sourceid1 = :sourceid1", array(':sourceid1' => $old_nid))->fetchField();
      $gids = og_get_group_ids('node', array($ding2libnid), array(OG_STATE_ACTIVE), FALSE);

      // Modify the node
      $node->field_ding_news_library['und'][0]['gid'] = $gids[$ding2libnid];
      $node->field_ding_news_library['und'][0]['state'] = 1;
      $node->field_ding_news_library['und'][0]['created'] = time();
      
      // Logging
      watchdog('migrate_ding1_ding2', 'Setting GID = %gid on news item (%title) (based on Ding2 library entity ID %ding2libnid - the Ding1 library nid was %old_nid)', array('%title'=>$node->title, '%gid' => $gids[$ding2libnid], '%ding2libnid' => $ding2libnid, '%old_nid' => $old_nid), WATCHDOG_INFO);
    }
  }
}


class DingEventMigration extends DingNodeMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Events from Ding1/D6'),
      'machine_name' => 'DingEvent',
      'source_type' => 'event',
      'destination_type' => 'ding_event',
      'dependencies' => array('DingUser','DingEventCategoryTerm','DingEventTargetTerm', 'DingTagsTerm', 'DingFile', 'DingLibrary'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['list_image_alt'] = t('Alt value extracted from field_list_image_data');
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    
    parent::__construct($input_arguments + $arguments);

    $this->addFieldMapping('field_ding_event_body', 'body');
    $this->addFieldMapping('field_ding_event_body:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_event_body:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_event_lead', 'field_teaser');
    $this->addFieldMapping('field_ding_event_lead:language')
         ->defaultValue('und');
    $this->addFieldMapping(NULL, 'field_library_ref')
         ->description(t('Handled in prepare function'));
    $this->addFieldMapping('field_ding_event_date', 'field_datetime');
    $this->addFieldMapping('field_ding_event_date:value2', 'field_datetime:value2');
    $this->addFieldMapping('field_ding_event_price', 'field_entry_price');

    // Files mapping
    $this->addFieldMapping('field_ding_event_files', 'field_file_attachments')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_event_files:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_event_files:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_event_files:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining page file fields
    $this->addFieldMapping('field_ding_event_files:description', 'field_file_attachments:description');
    $this->addFieldMapping('field_ding_event_files:display', 'field_file_attachments:description')
         ->description(t('Use field_file_attachments:description again for this value'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_event_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_event_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_event_list_image:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_event_list_image:preserve_files')
         ->defaultValue(FALSE);
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_event_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_event_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // Map to addressfield. See https://drupal.org/node/1996546.
    $this->addFieldMapping('field_ding_event_location', 'country');
    $this->addFieldMapping('field_ding_event_location:thoroughfare', 'street');
    $this->addFieldMapping('field_ding_event_location:premise', 'additional');
    $this->addFieldMapping('field_ding_event_location:locality', 'city');
    $this->addFieldMapping('field_ding_event_location:administrative_area', 'province');
    $this->addFieldMapping('field_ding_event_location:postal_code', 'postal_code');
    $this->addFieldMapping('field_ding_event_location:name_line', 'name');
    
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'latitude');
    $this->addFieldMapping(NULL, 'longitude');
    $this->addFieldMapping(NULL, 'lid');

    // ting references
    $this->addFieldMapping(NULL, 'field_ting_refs')
         ->description(t('Handled in complete function'));
    $this->addFieldMapping(NULL, 'field_ting_refs:ting_ref_type')
         ->description(t('Not mapped - Ding2 does not employ ref types'));
    $this->addFieldMapping(NULL, 'field_ting_refs:description')
         ->description(t('Not mapped - Ding2 does not employ descriptions on ting refs.'));

    // Event Category term mapping
    $this->addFieldMapping('field_ding_event_category', '4')
         ->sourceMigration('DingEventCategoryTerm');
    $this->addFieldMapping('field_ding_event_category:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_event_category:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_event_category:ignore_case')
         ->defaultValue(TRUE);

    // Event Target term mapping
    $this->addFieldMapping('field_ding_event_target', '5')
         ->sourceMigration('DingEventTargetTerm');
    $this->addFieldMapping('field_ding_event_target:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_event_target:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_event_target:ignore_case')
         ->defaultValue(TRUE);

         // Event Tags term mapping
    $this->addFieldMapping('field_ding_event_tags', '2')
         ->sourceMigration('DingTagsTerm');
    $this->addFieldMapping('field_ding_event_tags:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_event_tags:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_event_tags:ignore_case')
         ->defaultValue(TRUE);

    // Unmapped source fields
    $this->addFieldMapping(NULL, 'field_content_images')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_content_images:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'field_file_attachments:list')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->description(t('Migration of revisions is possible, but not implemented in this migration'))
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, '1')
         ->description(t('Ding event post category. Not implemented in ding2'))
         ->issueGroup('DNM');
    $this->addFieldMapping(NULL, 'field_kultunaut_agegroup')
         ->issueGroup('DNM');
    $this->addFieldMapping(NULL, 'field_kultunaut_category')
         ->issueGroup('DNM');

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(  
      'field_ding_event_title_image', 
      'field_ding_event_title_image:file_class', 
      'field_ding_event_title_image:language', 
      'field_ding_event_title_image:alt', 
      'field_ding_event_title_image:title',
      'field_ding_event_title_image:destination_dir', 
      'field_ding_event_title_image:destination_file', 
      'field_ding_event_title_image:file_replace', 
      'field_ding_event_title_image:preserve_files', 
      'field_ding_event_title_image:source_dir',
      'field_ding_event_title_image:urlencode',
      'field_ding_event_library',
      'field_ding_event_materials',
      'field_ding_event_library:state',
      'field_ding_event_library:created',
      'field_ding_event_location:sub_administrative_area',
      'field_ding_event_location:dependent_locality',
      'field_ding_event_location:sub_premise',
      'field_ding_event_location:organisation_name',
      'field_ding_event_location:first_name',
      'field_ding_event_location:last_name',
      'field_ding_event_location:data',
    ));

  }

  public function prepareRow($row) {
    // Make sure that this migration only runs if ding_place2book is turned OFF on the site
    if (module_exists('ding_place2book')) {
      throw new MigrateException(t('Turn ding_place2book module OFF before running DingEventMigration'));
    }
    
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Manually fetch the data subfield from the image fields and populate the
    // the image alt and title fields defined in our constructor.
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('content_field_list_image', 'cfli')
             ->fields('cfli', array('field_list_image_data'))
             ->condition('cfli.nid', $row->nid)
             ->condition('cfli.vid', $row->vid);
    $list_image_data = unserialize($query->execute()->fetchField());
    
    // Populate our fields defined in the constructor from the data in the 
    // fetched subfields. Since we dont have a delta in the table, we can treat
    // it as one row.
    if ($list_image_data) {
      $row->list_image_alt = $list_image_data['alt'];
      $row->list_image_title = $list_image_data['title'];      
    }
    
    // Handle date field
    $date_data = array(
      'from' => $row->field_datetime,
      'to' => $row->{'field_datetime:value2'},
    );
    // Datetime field handler accepts data as json array.
    $row->field_datetime = drupal_json_encode($date_data);
    
    // Make country code uppercase
    $row->country = strtoupper($row->country);

  }

  public function prepare($node, stdClass $row) {
    // Assign news to library by manipulating node object directly. 
    // Ugly, but needs to be done this way until we have bumped OG version to 2.1, which contains the migrate classes needed by OG
    $this->setDingEventGid($node, $row);
  }

  public function complete($node, stdClass $row) {
    // Create Ting objects and relation to it for each item i field_ting_refs
    // We perform this in complete() function since we need the nid on the news node just created
    parent::createDingNodeTingRefs($node, $row);
  }

  // function to set GID for a ding_event node
  private function setDingEventGid($node, $row) {
    $old_nid = $row->field_library_ref[0];

    // Process if old Ding1 node has a field_library_ref value
    if ($old_nid) {
      // Get new entity ID for the library from the mappings
      $ding2libnid = db_query("SELECT destid1 FROM {migrate_map_dinglibrary} WHERE sourceid1 = :sourceid1", array(':sourceid1' => $old_nid))->fetchField();
      $gids = og_get_group_ids('node', array($ding2libnid), array(OG_STATE_ACTIVE), FALSE);

      // Modify the node
      $node->field_ding_event_library['und'][0]['gid'] = $gids[$ding2libnid];
      $node->field_ding_event_library['und'][0]['state'] = 1;
      $node->field_ding_event_library['und'][0]['created'] = time();
      
      // Logging
      watchdog('migrate_ding1_ding2', 'Setting GID = %gid on event item (%title) (based on Ding2 library entity ID %ding2libnid - the Ding1 library nid was %old_nid)', array('%title'=>$node->title, '%gid' => $gids[$ding2libnid], '%ding2libnid' => $ding2libnid, '%old_nid' => $old_nid), WATCHDOG_INFO);
    }
  }

  // Modify the query to include location data
  // For more info, see:
  // http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture as well as
  // http://www.grasmash.com/article/migrate-classes-location-cck-address-field
  protected function query() {
    $query = parent::query();
    $query->leftJoin('location_instance', 'li', 'n.nid=li.nid AND n.vid = li.vid');
    $query->addField('li', 'lid');
    $query->leftJoin('location', 'l', 'li.lid=l.lid');
    $query->addField('l', 'name');
    $query->addField('l', 'street');
    $query->addField('l', 'additional');
    $query->addField('l', 'city');
    $query->addField('l', 'province');
    $query->addField('l', 'postal_code');
    $query->addField('l', 'country');
    $query->addField('l', 'latitude');
    $query->addField('l', 'longitude');
    return $query;
  }

}

