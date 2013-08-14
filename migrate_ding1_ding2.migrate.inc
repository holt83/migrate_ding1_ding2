<?php
/*
 * You must implement hook_migrate_api(), setting the API level to 2, if you are
 * implementing any migration classes. As of Migrate 2.5, you should also
 * register your migration and handler classes explicitly here - the former
 * method of letting them get automatically registered on a cache clear will
 * break in certain environments (see http://drupal.org/node/1778952).
 */
function migrate_ding1_ding2_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      // Node types
      'DingCampaign' => array('class_name' => 'DingCampaignMigration'),
      'DingEvent' => array('class_name' => 'DingEventMigration'),
      'DingLibrary' => array('class_name' => 'DingLibraryMigration'),
      'DingNews' => array('class_name' => 'DingNewsMigration'),
      'DingPage' => array('class_name' => 'DingPageMigration'),

      // User profiles
      'DingRole' => array('class_name' => 'DingRoleMigration'),
      'DingUser' => array('class_name' => 'DingUserMigration'),
      'DingProfile2' => array('class_name' => 'DingProfile2Migration'),

      // Taxonomies
      'DingEventCategoryTerm' => array('class_name' => 'DingEventCategoryTermMigration'),
      'DingEventTargetTerm' => array('class_name' => 'DingEventTargetTermMigration'),
      'DingNewsCategoryTerm' => array('class_name' => 'DingNewsCategoryTermMigration'),
      'DingTagsTerm' => array('class_name' => 'DingTagsTermMigration'),

      // Tables
      'DingPlace2book' => array('class_name' => 'DingPlace2bookMigration'),
      'DingOpeningHours' => array('class_name' => 'DingOpeningHoursMigration'),

      // Miscellaneous
      'DingFile' => array('class_name' => 'DingFileMigration'),
      'DingCart' => array('class_name' => 'DingCartMigration'),
      
    ),

    'field handlers' => array(
      'DateMigrateFieldHandler',
    ),
  );
  return $api;
}


/*
 * Function to insert a new ting_object into the database.
 * 
 * Return value: the tid of the created ting_object.
 * 
 * TO DO: This is not the drupal way of creating a new ting_object entity a should be replaced by the proper calls.
 */
function migrate_ding1_ding2_add_ting_object($ting_id) {
  // if ting_object already exists, return the tid
  $result = db_query('SELECT t.tid FROM {ting_object} t WHERE t.ding_entity_id = :entid', array(':entid' => $ting_id));
  $tid = $result->fetchField();
  if ($tid) {
    return $tid;
  }
  
  // open transaction
  $transaction = db_transaction();

  try {
    $tid = db_insert('ting_object')
      ->fields(array(
        'ding_entity_id' => $ting_id,
      ))
      ->execute();

    $vid = db_insert('ting_object_revision')
      ->fields(array(
        'tid' => $tid,
        'timestamp' => 0,
      ))
      ->execute();

    db_update('ting_object')
    ->condition('tid', $tid)
    ->fields(array('vid' => $vid))
    ->execute();
  }
  catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('migrate_ding1_ding2', $e);
  }
  
  return $tid;
}

