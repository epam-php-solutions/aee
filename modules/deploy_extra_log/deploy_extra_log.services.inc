<?php

/**
 * Implements hook_services_resources_alter().
 *
 */
function deploy_extra_log_services_resources_alter(&$resources, &$endpoint) {
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (isset($entity_info['uuid']) && $entity_info['uuid'] == TRUE && isset($resources[$entity_type])) {

      // Try to save function to .
      $old_callback = $resources[$entity_type]['operations']['update']['callback'];

      // We should change only update callback.
      $resources[$entity_type]['operations']['update']['callback'] = '_deploy_extra_log_update';
    }
  }
}

/**
 * Callback for the 'update' method.
 *
 * @see entity_uuid_save()
 */
function _deploy_extra_log_update($entity_type, $uuid, $entity) {

  // Check __metadata properties.
  if (isset($entity['__metadata']['deploy_extra']['adv_log']) && !empty($entity['__metadata']['deploy_extra']['adv_log'])) {
    // @TODO need to add our code for logging here
  }
  elseif (function_exists('_uuid_services_entity_update')) {
    // Execute native uuid_services function.
    _uuid_services_entity_update($entity_type, $uuid, $entity);
  }
  else {
    // If the function name was changed trying to do the work yourself.
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
    } catch (Exception $exception) {
      watchdog_exception('uuid_services', $exception);
      return services_error($exception, 406, $entity);
    }
  }
}

