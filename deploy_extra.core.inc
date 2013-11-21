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

  // Settings.
  $adv_log = (isset($config['adv_log']) && !empty($config['adv_log'])) ? TRUE : FALSE;
  $deployment_key = (isset($config['deployment_key']) && !empty($config['deployment_key'])) ? TRUE : FALSE;
  $tmode   = (isset($config['tmode']) && !empty($config['tmode'])) ? TRUE : FALSE;
  $skip    = (isset($config['skip']) && !empty($config['skip'])) ? TRUE : FALSE;

  // Perform logging process and return.
  if ($adv_log && $deployment_key && function_exists('deploy_extra_import_log')) {
    deploy_extra_import_log($entity_type, $uuid, $entity, $deployment_key);
  }

  // If enable tmode just do nothing more.
  if ($tmode) {
    return;
  }

  // If enable skip updates mode.
  if ($skip) {
    $id = entity_get_id_by_uuid($entity_type, array($uuid));

    // Entity already exists. In skip mode - do nothing.
    if (!empty($id[$uuid])) {
      return;
    }
  }

  // Perform update/create process.
  deploy_extra_import_entity_save($entity_type, $uuid, $entity, $config);
}

function deploy_extra_import_entity_save($entity_type, $uuid, $entity, $config) {

  // Add ability to change entity before executing save/update operation.
  drupal_alter('deploy_extra_import_entity', $entity_type, $uuid, $entity, $config);

  try {
    $controller = entity_get_controller($entity_type);
    if ($controller instanceof EntityAPIControllerInterface) {
      $entity = $controller->create($entity);
    }
    else {
      $entity = (object) $entity;
    }

    entity_uuid_save($entity_type, $entity);

    return $entity;
  }
  catch (Exception $exception) {
    watchdog_exception('deploy_extra', $exception);
    return services_error($exception, 406, $entity);
  }
}

/**
 * Helper function.
 * Save variables in to watchdog.
 *
 * @param $data
 */
function deploy_extra_debug($data) {
  watchdog('deploy_extra:debug', 'Debug vars: <pre>@info</pre>', array('@info' => print_r($data, 1)), WATCHDOG_DEBUG);
}
