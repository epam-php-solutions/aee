<?php

/**
 * Implements hook_services_resources_alter().
 *
 */
function deploy_extra_log_services_resources_alter(&$resources, &$endpoint) {
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (isset($entity_info['uuid']) && $entity_info['uuid'] == TRUE && isset($resources[$entity_type])) {

      // Try to save function to.
      // @TODO try to save function name in to static variable.
      $old_callback = $resources[$entity_type]['operations']['update']['callback'];

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
  $metadata = array(); $config = array();

  // To config from entity metadata.
  $to_config = deploy_extra_log_to_metadata();

  // Extract metadata from entity object.
  if(isset($entity['__metadata']['deploy_extra'])) {
    $metadata = $entity['__metadata']['deploy_extra'];
  }

  // Filling config array.
  foreach ($to_config as $property) {
    if(isset($metadata[$property])) {
      $to_config[$property] = (bool) $metadata[$property];
    }
  }

  deloy_extra_entity_update($entity_type, $uuid, $entity, $config);
}

