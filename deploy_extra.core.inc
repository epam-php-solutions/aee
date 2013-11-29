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
 */
function deloy_extra_import_entity($entity_type, $uuid, $entity, $config) {

  // Settings preparation.
  deploy_extra_import_settings_preparation($config);

  // Add ability to change entity before executing save/update operation.
  drupal_alter('deploy_extra_import_entity', $entity_type, $uuid, $entity, $config);

  // Is the entity exists?
  $id = entity_get_id_by_uuid($entity_type, array($uuid));
  $config['entity_exists'] = !empty($id[$uuid]);

  // If enable skip updates mode.
  if ($config['skip'] && $config['entity_exists']) {
    // Entity already exists. Enable tmode for this entity.
    $config['tmode'] = TRUE;
  }

  // Perform update/create process.
  deploy_extra_import_entity_save($entity_type, $entity, $uuid, $config);
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
  try {
    $controller = entity_get_controller($entity_type);
    if ($controller instanceof EntityAPIControllerInterface) {
      $entity = $controller->create($entity);
    }
    else {
      $entity = (object) $entity;
    }

    // Perform checking process.
    if (deploy_extra_adv_log_ability($config)) {
      // Create a clone of the entity object to check,
      // that forbid directly changing entity object.
      $clone = clone $entity;
      deploy_extra_import_check($entity_type, $uuid, $clone, $config['deployment_key']);
    }

    // Test mode.
    if (!empty($config['tmode'])) {
      $status = ($config['skip'] && $config['entity_exists']) ? DEPLOY_EXTRA_IMPORT_STATUS_SKIPPED : DEPLOY_EXTRA_IMPORT_STATUS_TEST_MODE;
    }
    else {
      // Perform save/update operation.
      entity_uuid_save($entity_type, $entity);
      $status = ($config['entity_exists']) ? DEPLOY_EXTRA_IMPORT_STATUS_UPDATED : DEPLOY_EXTRA_IMPORT_STATUS_SAVED;
    }

    // Save the status of the operation into the log.
    if (deploy_extra_adv_log_ability($config)) {
      _deploy_extra_update_status_log($entity_type, $entity, $uuid, $config, $status);
    }

  } catch (Exception $exception) {
    watchdog_exception('deploy_extra', $exception);

    // Save the status of the operation into the log.
    if (deploy_extra_adv_log_ability($config)) {
      _deploy_extra_update_status_log($entity_type, $entity, $uuid, $config, DEPLOY_EXTRA_IMPORT_STATUS_FAILED);
    }

    return services_error($exception, 406, $entity);
  }
}

/**
 * @param $entity_type
 * @param $entity
 * @param $uuid
 * @param $config
 * @param $status
 */
function _deploy_extra_update_status_log($entity_type, $entity, $uuid, $config, $status) {
  if (!empty($config['deployment_key'])) {
    // Check whether the entity supports revision.
    $vuuid = deploy_extra_get_vuuid_from_entity($entity, $entity_type);
    deploy_extra_import_log_status($config['deployment_key'], $uuid, $entity_type, $status, array(), $vuuid);
  }
}

/**
 * Helper function.
 * Import settings array preparations.
 *
 * @param $config
 */
function deploy_extra_import_settings_preparation(&$config) {
  $config['adv_log']        = !empty($config['adv_log']) ? TRUE : FALSE;
  $config['deployment_key'] = !empty($config['deployment_key']) ? $config['deployment_key'] : FALSE;
  $config['tmode']          = !empty($config['tmode']) ? TRUE : FALSE;
  $config['skip']           = !empty($config['skip']) ? TRUE : FALSE;
}

/**
 * Helper debug function.
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

/**
 * Helper function.
 * Check whether the import supports advanced logging.
 *
 * @param $config
 * array with import process settings.
 * @return bool
 */
function deploy_extra_adv_log_ability($config) {
  if($config['adv_log'] && $config['deployment_key'] && module_exists('deploy_extra_log')) {
    return TRUE;
  }
  return FALSE;
}