<?php

/**
 * Implements hook_services_resources_alter().
 *
 */
function deploy_extra_log_services_resources_alter(&$resources, &$endpoint) {
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (isset($entity_info['uuid']) && $entity_info['uuid'] == TRUE && isset($resources[$entity_type])) {

      // Try to save function name to static variable.
      $old_callback = $resources[$entity_type]['operations']['update']['callback'];
      _deploy_extra_log_old_save_callback($old_callback);

      // We should change only update callback.
      $resources[$entity_type]['operations']['update']['callback'] = '_deploy_extra_log_update';
    }
  }
}

/**
 * Callback for the 'update' method.
 *
 */
function _deploy_extra_log_update($entity_type, $uuid, $entity) {
  $config = array();

  // To config from entity metadata.
  $to_config = deploy_extra_log_to_metadata();

  // Extract metadata from entity object.
  if (isset($entity['__metadata']['deploy_extra'])) {
    $metadata = $entity['__metadata']['deploy_extra'];

    // Filling config array.
    foreach ($to_config as $property) {
      if (isset($metadata[$property])) {
        $config[$property] = $metadata[$property];
      }
    }

    // Perform update/save operations.
    deloy_extra_import_entity($entity_type, $uuid, $entity, $config);
  }

  // Use native function for update/save operations
  elseif (function_exists(_deploy_extra_log_old_save_callback())) {
    // Execute native uuid_services function.
    call_user_func(_deploy_extra_log_old_save_callback(), $entity_type, $uuid, $entity);

  } else {
    // just in case.
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
      watchdog_exception('uuid_services', $exception);
      return services_error($exception, 406, $entity);
    }
  }
}

/**
 * Just storing old save/update callback.
 *
 * @param bool $callback
 * @return bool
 */
function _deploy_extra_log_old_save_callback($callback = FALSE) {
  $old_callback = &drupal_static(__FUNCTION__);

  if (!isset($old_callback) || !empty($callback)) {
    $old_callback = $callback;
  }

  return $old_callback;
}

