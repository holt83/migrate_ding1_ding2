<?php

/**
 * Common mappings for the Drupal 6 node migrations.
 */
abstract class DingNodeMigration extends DrupalNode6Migration {
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
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of Libraries from Ding1/D6'),
      'machine_name' => 'DingLibrary',
      'group_name' => 'ding1_group',
      'source_type' => 'library',
      'destination_type' => 'ding_library',
      'dependencies' => array('DingUser','DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['list_image_alt'] = t('Alt text extracted from field_list_image_data');
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    $this->sourceFields['image_alt'] = t('Alt text extracted from field_image_data');
    $this->sourceFields['image_title'] = t('Title text extracted from field_image_data');
    
    parent::__construct($arguments);

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
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_library_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_library_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:data')
         ->description(t('Split this into list_image_alt and list_image_title for D7'));
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // Map the image field to the new title image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_library_title_image', 'field_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_library_title_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_library_title_image:language')
         ->defaultValue('und');
    // Mapping remaining title image fields
    $this->addFieldMapping('field_ding_library_title_image:alt', 'image_alt');
    $this->addFieldMapping('field_ding_library_title_image:title', 'image_title');
    $this->addFieldMapping(NULL, 'field_image:data')
         ->description(t('Split this into image_alt and image_title for D7'));
    $this->addFieldMapping(NULL, 'field_image:list');

    $this->addFieldMapping(NULL, 'field_content_images')
         ->description(t('content images are not mapped to Ding2/D7'));
    $this->addFieldMapping(NULL, 'field_content_images:list');
    $this->addFieldMapping(NULL, 'field_content_images:data');
    
    // Map to addressfield. See addressfield.inc in migrate_extras for available argument keys.
    $address_arguments = array(
      'thoroughfare' => array('source_field' => 'street'),
      'premise' => array('source_field' => 'additional'),
      'locality' => array('source_field' => 'city'),
      'administrative_area' => array('source_field' => 'province'),
      'postal_code' => array('source_field' => 'postal_code'),
      'name_line' => array('source_field' => 'name'),
    );
    // Note that of the country field is NULL, none of the values will be migrated!
    $this->addFieldMapping('field_ding_library_addresse', 'country')
         ->arguments($address_arguments);
 
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'name');
    $this->addFieldMapping(NULL, 'street');
    $this->addFieldMapping(NULL, 'additional');
    $this->addFieldMapping(NULL, 'city');
    $this->addFieldMapping(NULL, 'province');
    $this->addFieldMapping(NULL, 'postal_code');
    $this->addFieldMapping(NULL, 'country');
    $this->addFieldMapping(NULL, 'latitude');
    $this->addFieldMapping(NULL, 'longitude');
    $this->addFieldMapping(NULL, 'lid');
    
    // Map to geofield. See geofield.inc in migrate_extras for available argument keys.
    $geo_arguments = array(
      'lat' => array('source_field' => 'latitude'),
      'lon' => array('source_field' => 'longitude'),
    );
    $this->addFieldMapping('field_ding_library_geocode')
         ->defaultValue('Point')
         ->arguments($geo_arguments);
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'latitude');
    $this->addFieldMapping(NULL, 'longitude');

    // Mapping taxonomies
    $this->addFieldMapping(NULL, '2');

    // opening_hours fields relates to the module office_hours in Ding1/D6, and should not be mapped - this module is not used i Ding2/D7
    $this->addFieldMapping(NULL, 'field_opening_hours')
         ->description(t('Not mapped - Ding2/D7 uses the opening_hours module instead of office_hours module. This is handled by separate migration of the opening_hours table'));
    $this->addFieldMapping(NULL, 'field_opening_hours:week_start');
    $this->addFieldMapping(NULL, 'field_opening_hours:week_end');
    $this->addFieldMapping(NULL, 'field_opening_hours:sun_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:sun_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:mon_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:mon_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:tue_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:tue_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:wed_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:wed_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:thu_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:thu_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:fri_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:fri_close');
    $this->addFieldMapping(NULL, 'field_opening_hours:sat_open');
    $this->addFieldMapping(NULL, 'field_opening_hours:sat_close');

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

    // Misc. mappings
    $this->addFieldMapping(NULL, 'field_pc_booking_method')
         ->description(t('Ding2/D7 does not employ pc booking'));
    $this->addFieldMapping(NULL, 'field_pc_booking_url')
         ->description(t('Ding2/D7 does not employ pc booking'));
    $this->addFieldMapping(NULL, 'field_feature_ref')
         ->description(t('Ding2/D7 does not employ feature nodes'));

  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }

    // Get values from serialized data and populate the source fields defined in our constructor
    $list_image_data = unserialize($row->field_list_image_data);
    $row->list_image_alt = $list_image_data['alt'];
    $row->list_image_title = $list_image_data['title'];
    
    $image_data = unserialize($row->field_image_data);
    $row->image_alt = $image_data['alt'];
    $row->image_title = $image_data['title'];

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
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of Campaigns from Ding1/D6'),
      'machine_name' => 'DingCampaign',
      'group_name' => 'ding1_group',
      'source_type' => 'campaign',
      'destination_type' => 'ding_campaign',
      'dependencies' => array('DingUser'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['campaign_image_alt'] = t('Alt text extracted from field_campaign_image_data');
    $this->sourceFields['campaign_image_title'] = t('Title text extracted from field_campaign_image_data');
    
    parent::__construct($arguments);

    $this->addFieldMapping('field_camp_text_full', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_text_plain', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_link', 'field_campaign_link');
    $this->addFieldMapping('field_camp_image:alt', 'campaign_image_alt');
    $this->addFieldMapping('field_camp_image:title', 'campaign_image_title');
    $this->addFieldMapping(NULL, 'field_campaign_image:data')
         ->description(t('Split this into campaign_image_alt and campaign_image_title for D7'));
    $this->addFieldMapping(NULL, '2')
         ->description(t('Ding2 does not employ the Tags taxonomy (taxonomy "2") for campaigns'));
    $this->addFieldMapping(NULL, 'field_campaign_link:title')
         ->description(t('Ding2 does not have title data on the campaign links'));
    $this->addFieldMapping(NULL, 'field_campaign_link:attributes')
         ->description(t('Ding2 does not have attribute data on the campaign links'));
    $this->addFieldMapping('field_camp_weight', 'campaign_weight');
    $this->addFieldMapping('field_camp_settings', 'campaign_type')
         ->description(t('Handled in prepareRow of DingCampaignMigration'));
    $this->addFieldMapping('field_camp_link:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_full:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_plain:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_full:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping(NULL, 'field_campaign_image:list');
         
    // Map the image field to the new image field, base this on the Files migration
    // to map the files to the imported files table.
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
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'log')
         ->issueGroup(t('DNM'));
    $this->addFieldMapping(NULL, 'revision_uid')
         ->issueGroup(t('DNM'));
  }
  
  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Get values from serialized data and populate the source fields defined in our constructor
    $campaign_image_data = unserialize($row->field_campaign_image_data);
    $row->campaign_image_alt = $campaign_image_data['alt'];
    $row->campaign_image_title = $campaign_image_data['title'];
    
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
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of Pages from Ding1/D6'),
      'machine_name' => 'DingPage',
      'group_name' => 'ding1_group',
      'source_type' => 'page',
      'destination_type' => 'ding_page',
      'dependencies' => array('DingUser','DingTagsTerm', 'DingLibrary', 'DingFile'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['file_attachments_description'] = t('Description text extracted from field_file_attachments_data');
    $this->sourceFields['list_image_alt'] = t('Alt text extracted from field_list_image_data');
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    
    parent::__construct($arguments);

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
    // Mapping remaining page file fields
    $this->addFieldMapping('field_ding_page_files:description', 'file_attachments_description');
    $this->addFieldMapping(NULL, 'field_file_attachments:data')
         ->description(t('Split this into description for D7'));
    $this->addFieldMapping('field_ding_page_files:display', 'file_attachments_description')
         ->description(t('Use file_attachments_description again for this value - nothing else to map from source'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_page_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_page_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_page_list_image:language')
         ->defaultValue('und');
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_page_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_page_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:data')
         ->description(t('Split this into list_image_alt and list_image_title for D7'));
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // ting references
    $this->addFieldMapping(NULL, 'field_ting_refs')
         ->description(t('Handled in complete function'));
    $this->addFieldMapping(NULL, 'field_ting_refs:ting_ref_type')
         ->description(t('Not mapped - Ding2 does not employ ref types'));
    $this->addFieldMapping(NULL, 'field_ting_refs:description')
         ->description(t('Not mapped - Ding2 does not employ descriptions on ting refs.'));

    // Tags term mapping
    $this->addFieldMapping('field_ding_page_tags', '2')
         ->sourceMigration('DingTagsTerm');
    $this->addFieldMapping('field_ding_page_tags:source_type')
         ->defaultValue('tid');
    $this->addFieldMapping('field_ding_page_tags:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_ding_page_tags:ignore_case')
         ->defaultValue(TRUE);

    // Add null mappings so sources are not flagged as unmapped.
    $this->addFieldMapping(NULL, 'field_content_images');
    $this->addFieldMapping(NULL, 'field_content_images:list');
    $this->addFieldMapping(NULL, 'field_content_images:data');
    $this->addFieldMapping(NULL, 'field_file_attachments:list');
    $this->addFieldMapping(NULL, 'field_library_ref')
         ->description(t('Not mapped - Ding2 does not relate pages to libraries'));

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
      'field_ding_page_materials',
    ));
    
  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Get values from serialized data and populate the source fields defined in our constructor
    // file attachments
    $file_attachments_data = unserialize($row->field_file_attachments_data);
    $row->file_attachments_description = $file_attachments_data['description'];

    // list images
    $list_image_data = unserialize($row->field_list_image_data);
    $row->list_image_alt = $list_image_data['alt'];
    $row->list_image_title = $list_image_data['title'];
  }

  public function complete($node, stdClass $row) {
    // Create Ting objects and relation to it for each item i field_ting_refs
    // We perform this in complete() function since we need the nid on the news node just created
    parent::createDingNodeTingRefs($node, $row);
  }
  
}


class DingNewsMigration extends DingNodeMigration {
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of Blogposts/News from Ding1/D6'),
      'machine_name' => 'DingNews',
      'group_name' => 'ding1_group',
      'source_type' => 'article',
      'destination_type' => 'ding_news',
      'dependencies' => array('DingUser','DingNewsCategoryTerm','DingTagsTerm','DingFile','DingLibrary'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['file_attachments_description'] = t('Description text extracted from field_file_attachments_data');
    $this->sourceFields['list_image_alt'] = t('Alt text extracted from field_list_image_data');
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');

    parent::__construct($arguments);

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
    // Mapping remaining page file fields
    $this->addFieldMapping('field_ding_news_files:description', 'file_attachments_description');
    $this->addFieldMapping(NULL, 'field_file_attachments:data')
         ->description(t('Split this into description for D7'));
    $this->addFieldMapping('field_ding_news_files:display', 'file_attachments_description')
         ->description(t('Use file_attachments_description again for this value - nothing else to map from source'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_news_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_news_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_news_list_image:language')
         ->defaultValue('und');
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_news_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_news_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:data')
         ->description(t('Split this into list_image_alt and list_image_title for D7'));
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // ting references
    $this->addFieldMapping(NULL, 'field_ting_refs')
         ->description(t('Handled in complete function'));
    $this->addFieldMapping(NULL, 'field_ting_refs:ting_ref_type')
         ->description(t('Not mapped - Ding2 does not employ ref types'));
    $this->addFieldMapping(NULL, 'field_ting_refs:description')
         ->description(t('Not mapped - Ding2 does not employ descriptions on ting refs.'));

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

    // Add null mappings so sources are not flagged as unmapped.
    $this->addFieldMapping(NULL, 'field_content_images');
    $this->addFieldMapping(NULL, 'field_content_images:list');
    $this->addFieldMapping(NULL, 'field_content_images:data');
    $this->addFieldMapping(NULL, 'field_file_attachments:list');

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
      'field_ding_news_library',
      'field_ding_news_materials',
    ));

  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Get values from serialized data and populate the source fields defined in our constructor
    // file attachments
    $file_attachments_data = unserialize($row->field_file_attachments_data);
    $row->file_attachments_description = $file_attachments_data['description'];

    // list images
    $list_image_data = unserialize($row->field_list_image_data);
    $row->list_image_alt = $list_image_data['alt'];
    $row->list_image_title = $list_image_data['title'];
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
  public function __construct($arguments) {
    
    $arguments = array(
      'description' => t('Migration of Events from Ding1/D6'),
      'machine_name' => 'DingEvent',
      'group_name' => 'ding1_group',
      'source_type' => 'event',
      'destination_type' => 'ding_event',
      'dependencies' => array('DingUser','DingEventCategoryTerm','DingEventTargetTerm', 'DingTagsTerm', 'DingFile', 'DingLibrary'),
    );

    // Add to sourceFields before calling parent constructor. 
    // See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['file_attachments_description'] = t('Description text extracted from field_file_attachments_data');
    $this->sourceFields['list_image_alt'] = t('Alt text extracted from field_list_image_data');
    $this->sourceFields['list_image_title'] = t('Title text extracted from field_list_image_data');
    
    parent::__construct($arguments);

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
    $this->addFieldMapping('field_ding_event_date', 'field_datetime')
         ->description(t('Handled in prepareRow function'));
    // Add NULL mapping so to-date is not flagged as unmapped
    $this->addFieldMapping(NULL, 'field_datetime:value2');
    $this->addFieldMapping('field_ding_event_price', 'field_entry_price');

    // Files mapping
    $this->addFieldMapping('field_ding_event_files', 'field_file_attachments')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_event_files:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_event_files:language')
         ->defaultValue('und');
    // Mapping remaining page file fields
    $this->addFieldMapping('field_ding_event_files:description', 'file_attachments_description');
    $this->addFieldMapping(NULL, 'field_file_attachments:data')
         ->description(t('Split this into description for D7'));
    $this->addFieldMapping('field_ding_event_files:display', 'file_attachments_description')
         ->description(t('Use file_attachments_description again for this value - nothing else to map from source'));

    // Map the list image field to the new list image field, base this on the DingFile migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_ding_event_list_image', 'field_list_image')
         ->sourceMigration('DingFile');
    $this->addFieldMapping('field_ding_event_list_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_ding_event_list_image:language')
         ->defaultValue('und');
    // Mapping remaining list image fields
    $this->addFieldMapping('field_ding_event_list_image:alt', 'list_image_alt');
    $this->addFieldMapping('field_ding_event_list_image:title', 'list_image_title');
    $this->addFieldMapping(NULL, 'field_list_image:data')
         ->description(t('Split this into list_image_alt and list_image_title for D7'));
    $this->addFieldMapping(NULL, 'field_list_image:list');

    // Map to addressfield. See addressfield.inc in migrate_extras for available argument keys.
    $address_arguments = array(
      'thoroughfare' => array('source_field' => 'street'),
      'premise' => array('source_field' => 'additional'),
      'locality' => array('source_field' => 'city'),
      'administrative_area' => array('source_field' => 'province'),
      'postal_code' => array('source_field' => 'postal_code'),
      'name_line' => array('source_field' => 'name'),
    );
    // Note that of the country field is NULL, none of the values will be migrated!
    $this->addFieldMapping('field_ding_event_location', 'country')
         ->arguments($address_arguments);
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'name');
    $this->addFieldMapping(NULL, 'street');
    $this->addFieldMapping(NULL, 'additional');
    $this->addFieldMapping(NULL, 'city');
    $this->addFieldMapping(NULL, 'province');
    $this->addFieldMapping(NULL, 'postal_code');
    $this->addFieldMapping(NULL, 'country');
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

    // Add null mappings so sources are not flagged as unmapped.
    $this->addFieldMapping(NULL, 'field_content_images');
    $this->addFieldMapping(NULL, 'field_content_images:list');
    $this->addFieldMapping(NULL, 'field_content_images:data');
    $this->addFieldMapping(NULL, 'field_file_attachments:list');

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
      'field_ding_event_library',
      'field_ding_event_materials',
    ));

  }

  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Get values from serialized data and populate the source fields defined in our constructor
    // file attachments
    $file_attachments_data = unserialize($row->field_file_attachments_data);
    $row->file_attachments_description = $file_attachments_data['description'];

    // list images
    $list_image_data = unserialize($row->field_list_image_data);
    $row->list_image_alt = $list_image_data['alt'];
    $row->list_image_title = $list_image_data['title'];
    
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

