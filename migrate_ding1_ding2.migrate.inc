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

      // Miscellaneous
      'DingFile' => array('class_name' => 'DingFileMigration'),
    ),

    'field handlers' => array(
      'DateMigrateFieldHandler',
    ),
  );
  return $api;
}
