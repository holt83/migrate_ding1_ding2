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

    // include the copy of link.migrate.inc from http://drupalcode.org/project/link.git/blob_plain/48f6229416990e42de7d3638933da3319075cd33:/link.migrate.inc 
    // until ding2 makefile bumbs Link module to version 1.1
    // When this happens, this snippet (and the corresponding class below) should be removed
    'field handlers' => array(
      'MigrateLinkFieldHandler',
    ),

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
      'DingFile' => array('class_name' => 'DingFileMigration'),
    ),
  );
  return $api;
}


/**
 * With Migrate 2.4 or later, you can use the subfield syntax to set the title
 * and attributes:
 *
 * @code
 * $this->addFieldMapping('field_my_link', 'source_url');
 * $this->addFieldMapping('field_my_link:title', 'source_title');
 * $this->addFieldMapping('field_my_link:attributes', 'source_attributes');
 * @endcode
 *
 * With earlier versions of Migrate, you must pass an arguments array:
 *
 * @code
 * $link_args = array(
 *   'title' => array('source_field' => 'source_title'),
 *   'attributes' => array('source_field' => 'source_attributes'),
 * );
 * $this->addFieldMapping('field_my_link', 'source_url')
 *      ->arguments($link_args);
 * @endcode
 */
class MigrateLinkFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('link_field'));
  }

  static function arguments($title = NULL, $attributes = NULL, $language = NULL) {
    $arguments = array();
    if (!is_null($title)) {
      $arguments['title'] = $title;
    }
    if (!is_null($attributes)) {
      $arguments['attributes'] = $attributes;
    }
    if (!is_null($language)) {
      $arguments['language'] = $language;
    }
    return $arguments;
  }

  /**
   * Implementation of MigrateFieldHandler::fields().
   *
   * @param $type
   *  The field type.
   * @param $instance
   *  Instance info for the field.
   * @param Migration $migration
   *  The migration context for the parent field. We can look at the mappings
   *  and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $instance, $migration = NULL) {
    return array(
      'title' => t('Subfield: The link title attribute'),
      'attributes' => t('Subfield: The attributes for this link'),
      'language' => t('Subfield: The language for the field'),
    );
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }
    else {
      $arguments = array();
    }

    $language = $this->getFieldLanguage($entity, $field_info, $arguments);
    $values = array_filter($values);

    foreach ($values as $delta => $value) {
      $item = array();
      if (isset($arguments['title'])) {
        if (!is_array($arguments['title'])) {
          $item['title'] = $arguments['title'];
        }
        elseif (isset($arguments['title'][$delta])) {
          $item['title'] = $arguments['title'][$delta];
        }
      }
      if (isset($arguments['attributes'])) {
        $item['attributes'] = $arguments['attributes'];
      }
      $item['url'] = $value;
      $return[$language][$delta] = $item;
    }

    return isset($return) ? $return : NULL;
  }
}