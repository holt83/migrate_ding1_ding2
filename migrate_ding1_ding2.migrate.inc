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
      //'DingTopic' => array('class_name' => 'DingTopicMigration'),
      // User profiles
      //'DingProfile' => array('class_name' => 'DingProfileMigration'),

      // Taxonomies
      'DingEventCategoryTerm' => array('class_name' => 'DingEventCategoryTermMigration'),
      'DingEventTargetTerm' => array('class_name' => 'DingEventTargetTermMigration'),
      'DingNewsCategoryTerm' => array('class_name' => 'DingNewsCategoryTermMigration'),
      'DingTagsTerm' => array('class_name' => 'DingTagsTermMigration'),

       // Miscellaneous
      //'DingLocation' => array('class_name' => 'DingLocationMigration'),
      'DingFile' => array('class_name' => 'DingFileMigration'),
    ),
  );
  return $api;
}