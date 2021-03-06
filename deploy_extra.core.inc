<?php

/**
 * @file
 * Describe core functions provided by the Deploy Extra module.
 */

/**
 * The entry point for the import process.
 *
 * @param $entity_type
 * @param $uuid
 * @param $entity
 * @param $config
 * @return mixed
 */
function deploy_extra_import_entity($entity_type, $uuid, $entity, $config) {

  // Settings preparation.
  deploy_extra_import_settings_preparation($config);

  // Add ability to change entity before executing save/update operation.
  drupal_alter('deploy_extra_import_entity', $entity_type, $uuid, $entity, $config);

  // Is the entity exists?
  $id = entity_get_id_by_uuid($entity_type, array($uuid));
  $config['entity_exists'] = !empty($id[$uuid]);

  // Add default import callback.
  $callbacks[] = DEPLOY_EXTRA_IMPORT_DEFAULT_CALLBACK;

  // Get all callbacks for the import process.
  foreach (module_implements('deploy_extra_import_callbacks') as $module) {
    $function = $module . '_deploy_extra_import_callbacks';
    // Modules can change the $config and add their own callback functions.
    $function($callbacks, $config, $uuid, $entity_type, $entity);
  }

  $status = DEPLOY_EXTRA_IMPORT_STATUS_PROCESSED;

  try {
    $controller = entity_get_controller($entity_type);
    if ($controller instanceof EntityAPIControllerInterface) {
      $entity = $controller->create($entity);
    }
    else {
      $entity = (object) $entity;
    }

    // Calling all modules import callbacks.
    foreach ($callbacks as $callback) {
      $status = $callback($entity_type, $entity, $uuid, $config);
    }

  } catch (Exception $exception) {
    watchdog_exception('deploy_extra', $exception);
    $status = DEPLOY_EXTRA_IMPORT_STATUS_FAILED;
  }

  // Allow modules to do something after the deploy process has been finished.
  module_invoke_all('deploy_extra_import_complete', $entity_type, $entity, $uuid, $config, $status);

  // Return error if deploy has been failed.
  if ($status == DEPLOY_EXTRA_IMPORT_STATUS_FAILED) {
    return services_error($status['error'], 406, $entity);
  }
}

/**
 * Implements hook_deploy_extra_import_callbacks().
 */
function deploy_extra_deploy_extra_import_callbacks(&$callbacks, &$config, $uuid, $entity_type) {

  // If enabled mode to skip updates and entity already exists or enabled tmode.
  if ($config['tmode'] || ($config['skip'] && !empty($config['entity_exists']))) {
    // Remove default (save) callback for the test mode.
    foreach ($callbacks as $id => $callback) {
      if ($callback == DEPLOY_EXTRA_IMPORT_DEFAULT_CALLBACK) {
        // Remove save callback.
        unset($callbacks[$id]);
        return ($config['skip']) ? DEPLOY_EXTRA_IMPORT_STATUS_SKIPPED : DEPLOY_EXTRA_IMPORT_STATUS_TEST_MODE;
      }
    }
  }
}

/**
 * Check, save, enable entity.
 * @TODO add right description.
 *
 * @param $entity_type
 * @param $entity
 * @param $uuid
 * @param $config
 * @return mixed|object
 */
function deploy_extra_import_entity_save($entity_type, $entity, $uuid, $config) {
  // Perform the save/update operation.
  entity_uuid_save($entity_type, $entity);
  $status = ($config['entity_exists']) ? DEPLOY_EXTRA_IMPORT_STATUS_UPDATED : DEPLOY_EXTRA_IMPORT_STATUS_SAVED;

  return $status;
}

/**
 * Helper function.
 * Import settings array preparations.
 *
 * @param $config
 */
function deploy_extra_import_settings_preparation(&$config) {
  $config['adv_log'] = !empty($config['adv_log']) ? TRUE : FALSE;
  $config['deployment_key'] = !empty($config['deployment_key']) ? $config['deployment_key'] : FALSE;
  $config['tmode'] = !empty($config['tmode']) ? TRUE : FALSE;
  $config['skip'] = !empty($config['skip']) ? TRUE : FALSE;
}

/**
 * Helper debug function.
 * Save variables in to watchdog.
 *
 * @param $data
 */
function deploy_extra_debug($data, $func = '') {
  $prefix = '';
  if (!empty($func)) {
    $prefix = 'Function:' . $func . '<br> ';
  }

  watchdog('deploy_extra:debug', $prefix . 'Debug vars: <pre>@info</pre>', array('@info' => print_r($data, 1)), WATCHDOG_DEBUG);
}

/*
 * Helper function.
 * Return vuuid from entity.
 */
function deploy_extra_get_vuuid_from_entity($entity, $entity_type) {
  $info = entity_get_info($entity_type);

  if (!empty($info['entity keys']['revision uuid'])) {
    // Get the keys for revision UUID.
    $vuuid_key = $info['entity keys']['revision uuid'];

    // Fetch the local revision ID by its UUID.
    if (isset($entity->{$vuuid_key})) {
      return $entity->{$vuuid_key};
    }
  }

  return FALSE;
}