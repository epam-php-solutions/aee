<?php

/**
 * @file
 * Describe core functions provided by the Deploy Extra module.
 */

/**
 * Deployment statuses. DEL - Deploy Extra Log
 */
define('DEL_IMPORT_STATUS_PROCESSED', 0);
define('DEL_IMPORT_STATUS_SAVED', 1);
define('DEL_IMPORT_STATUS_UPDATED', 2);
define('DEL_IMPORT_STATUS_SKIPPED', 3);
define('DEL_IMPORT_STATUS_TEST_MODE', 4);
define('DEL_IMPORT_STATUS_FAILED', 5);

/**
 * Import function on the goal side.
 * @TODO add right description.
 *
 * @param $entity_type
 * @param $uuid
 * @param $entity
 * @param $config
 */
function deloy_extra_import_entity($entity_type, $uuid, $entity, $config) {

  // Settings preparation.
  deploy_extra_import_settings_preparation($config);

  // Add ability to change entity before executing save/update operation.
  drupal_alter('deploy_extra_import_entity', $entity_type, $uuid, $entity, $config['deployment_key']);

  // If enable skip updates mode.
  if ($config['skip']) {
    $id = entity_get_id_by_uuid($entity_type, array($uuid));

    // Entity already exists. Enable tmode for this entity.
    if (!empty($id[$uuid])) {
      $config['tmode'] = TRUE;
    }
  }

  // Perform update/create process.
  deploy_extra_import_entity_save($entity_type, $entity, $uuid, $config);
}

function deploy_extra_import_entity_save($entity_type, $entity, $uuid, $config) {
  try {
    $controller = entity_get_controller($entity_type);
    if ($controller instanceof EntityAPIControllerInterface) {
      $entity = $controller->create($entity);
    }
    else {
      $entity = (object) $entity;
    }

    // Perform logging process and return.
    if ($config['adv_log'] && $config['deployment_key'] && function_exists('deploy_extra_import_check')) {
      deploy_extra_import_check($entity_type, $uuid, $entity, $config['deployment_key']);
    }

    // Perform save/update operation.
    if(!$config['tmode']) {
      entity_uuid_save($entity_type, $entity);
    }

    return $entity;
  }
  catch (Exception $exception) {
    watchdog_exception('deploy_extra', $exception);
    return services_error($exception, 406, $entity);
  }
}

/**
 * Helper function.
 * Fills the an array of settings.
 *
 * @param $config
 */
function deploy_extra_import_settings_preparation(&$config) {
  $config['adv_log']        = (isset($config['adv_log']) && !empty($config['adv_log'])) ? TRUE : FALSE;
  $config['deployment_key'] = (isset($config['deployment_key']) && !empty($config['deployment_key'])) ? $config['deployment_key'] : FALSE;
  $config['tmode']          = (isset($config['tmode']) && !empty($config['tmode'])) ? TRUE : FALSE;
  $config['skip']           = (isset($config['skip']) && !empty($config['skip'])) ? TRUE : FALSE;
}

/**
 * Helper function.
 * Save variables in to watchdog.
 *
 * @param $data
 */
function deploy_extra_debug($data, $func = '') {
  $prefix = '';
  if(!empty($func)) {
    $prefix = 'Function:' . $func . '<br> ';
  }

  watchdog('deploy_extra:debug', $prefix . 'Debug vars: <pre>@info</pre>', array('@info' => print_r($data, 1)), WATCHDOG_DEBUG);
}

/**
 * helper function for logging of import entities process
 *
 * @param string $deployment_uuid
 * The UUID of deployment
 *
 * @param string $entity_uuid
 * The UUID of import entity
 *
 * @param string $entity_type
 * The type of import entity
 *
 * @param integer $status
 * The status of import entity, sed define above
 *
 * @param string $entity_vuuid
 * The UUID of entity revision if it exists
 *
 * @param string $message
 * The message for logging
 */

function _deploy_extra_import_log($deployment_uuid, $entity_uuid, $entity_type, $status = 0, $data = array(), $entity_vuuid = NULL) {

  // @TODO need to add function to transform $data array to $message string
  $message = NULL;
  if(isset($data['message'])) {
    $message = $data['message'];
  }

  $keys = array(
    'deployment_uuid' => $deployment_uuid,
    'entity_uuid' => $entity_uuid,
  );
  $fields = array(
    'deployment_uuid' => $deployment_uuid,
    'entity_uuid' => $entity_uuid,
    'entity_type' => $entity_type,
    'status' => $status,
    'timestamp' => time(),
    'data' => !empty($data) ? serialize($data) : NULL,
    'message' => $message,
  );

  if ($entity_vuuid) {
    $keys['entity_vuuid'] = $entity_vuuid;
    $fields['entity_vuuid'] = $entity_vuuid;
  }

  db_merge('deploy_extra_import_log')
    ->key($keys)
    ->fields($fields)
    ->execute();
}

/**
 * @TODO add the proper description
 */

function deploy_extra_import_log_status($deployment_uuid, $entity_uuid, $entity_type, $status, $data = array(), $entity_vuuid = NULL) {
  _deploy_extra_import_log($deployment_uuid, $entity_uuid, $entity_type, $status, $data, $entity_vuuid);
}

/**
 * @TODO add the proper description
 */

function deploy_extra_import_log($deployment_uuid, $entity_uuid, $entity_type, $data = array(), $entity_vuuid = NULL) {
  $status = DEL_IMPORT_STATUS_PROCESSED;
  _deploy_extra_import_log($deployment_uuid, $entity_uuid, $entity_type, $status, $data, $entity_vuuid);
}
