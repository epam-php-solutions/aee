<?php

/**
 * Implements hook_services_resources_alter().
 *
 */
function deploy_extra_services_resources_alter(&$resources, &$endpoint) {
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (!empty($entity_info['uuid']) && isset($resources[$entity_type])) {

      // Try to save old callback function name to variable.
      $old_callback = $resources[$entity_type]['operations']['update']['callback'];
      _deploy_extra_services_old_update_callback($old_callback);

      // We should change only update callback.
      $resources[$entity_type]['operations']['update']['callback'] = '_deploy_extra_services_update';
    }
  }
}

/**
 * Callback for the 'update' method.
 *
 */
function _deploy_extra_services_update($entity_type, $uuid, $entity) {
  $config = array();

  // To config from entity metadata.
  $to_config = deploy_extra_to_metadata();

  // Old services update/save callback.
  $old_callback = _deploy_extra_services_old_update_callback();

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

  // Use native function for update/save operations.
  elseif (function_exists($old_callback)) {
    // Execute native uuid_services function.
    call_user_func($old_callback, $entity_type, $uuid, $entity);

  }
  else {
    // Just in case.
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
function _deploy_extra_services_old_update_callback($callback = FALSE) {

  if (!empty($callback)) {
    variable_set('deploy_extra_services_old_update_callback', $callback);
    return $callback;
  }

  return variable_get('deploy_extra_services_old_update_callback', '_uuid_services_entity_update');
}
