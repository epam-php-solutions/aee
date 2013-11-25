<?php

/**
 * @file
 * Describe core functions provided by the Deploy Extra module.
 */

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
      // Create a clone of the entity object to check,
      // that forbid directly changing entity object.
      $clone = clone $entity;
      deploy_extra_import_check($entity_type, $uuid, $clone, $config['deployment_key']);
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
  $config['adv_log']        = !empty($config['adv_log']) ? TRUE : FALSE;
  $config['deployment_key'] = !empty($config['deployment_key']) ? $config['deployment_key'] : FALSE;
  $config['tmode']          = !empty($config['tmode']) ? TRUE : FALSE;
  $config['skip']           = !empty($config['skip']) ? TRUE : FALSE;
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
