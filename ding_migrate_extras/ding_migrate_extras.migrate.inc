<?php

/*
 * Implementation of hook_migrate_api().
 */
function ding_migrate_extras_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array(
      'MigrateLinkFieldHandler',
    )
  );
  return $api;
}
