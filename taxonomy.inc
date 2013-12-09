<?php

/**
 * Common mappings for the Drupal 6 node migrations.
 */
abstract class DingTermMigration extends DrupalTerm6Migration {  
  public function __construct(array $input_arguments) {
    $arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'soft_dependencies' => array('DingUser'),
    );
    
    parent::__construct($input_arguments + $arguments);

  }
}


class DingNewsCategoryTermMigration extends DingTermMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Category/News Category Terms from Ding1/D6'),
      'machine_name' => 'DingNewsCategoryTerm',
      'source_vocabulary' => '1',  // D6 Vocabulary ID - this may be different on the exact site and needs to be checked. 
      'destination_vocabulary' => 'news_category', // There is a "category" on a ding2 site, but this is not used. Blog posts in Ding1 correspond to News in Ding2.
    );

    parent::__construct($input_arguments + $arguments);

  }
}


class DingEventCategoryTermMigration extends DingTermMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Event Category Terms from Ding1/D6'),
      'machine_name' => 'DingEventCategoryTerm',
      'source_vocabulary' => '4',  // D6 Vocabulary ID - this may be different on the exact site and needs to be checked. 
      'destination_vocabulary' => 'event_category',
    );

    parent::__construct($input_arguments + $arguments);

  }
}


class DingEventTargetTermMigration extends DingTermMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Event Target Terms from Ding1/D6'),
      'machine_name' => 'DingEventTargetTerm',
      'source_vocabulary' => '5',  // D6 Vocabulary ID - this may be different on the exact site and needs to be checked. 
      'destination_vocabulary' => 'event_target',
    );

    parent::__construct($input_arguments + $arguments);

  }
}


class DingTagsTermMigration extends DingTermMigration {
  public function __construct($input_arguments) {
    
    $arguments = array(
      'description' => t('Migration of Tags Terms from Ding1/D6'),
      'machine_name' => 'DingTagsTerm',
      'source_vocabulary' => '2',  // D6 Vocabulary ID - this may be different on the exact site and needs to be checked. 
      'destination_vocabulary' => 'ding_content_tags',
    );

    parent::__construct($input_arguments + $arguments);

  }
}
