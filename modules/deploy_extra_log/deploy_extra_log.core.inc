<?php

/**
 * @file
 * Deploy Extra log module.
 */

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
function _deploy_extra_import_log($deployment_key, $uuid, $entity_type, $status, $data, $entity_vuuid) {

  $keys = array(
    'deployment_uuid' => $deployment_key,
    'entity_uuid' => $uuid,
  );

  $fields = array(
    'deployment_uuid' => $deployment_key,
    'entity_uuid' => $uuid,
    'entity_type' => $entity_type,
    'status' => $status,
    'timestamp' => time(),
  );

  $update_fields = array(
    'status' => $status,
    'timestamp' => time(),
  );

  if (is_array($data) && $status === DEPLOY_EXTRA_IMPORT_STATUS_PROCESSED) {
    $fields['data'] = $update_fields['data'] = !empty($data) ? serialize($data) : NULL;
  }

  if ($entity_vuuid) {
    $keys['entity_vuuid'] = $fields['entity_vuuid'] = $entity_vuuid;
  }

  db_merge(DEPLOY_EXTRA_LOG_TABLE_NAME)
    ->key($keys)
    ->insertFields($fields)
    ->updateFields($update_fields)
    ->execute();
}

/**
 * Updating the entity import status.
 */
function deploy_extra_import_log_status($deployment_key, $uuid, $entity_type, $status, $entity_vuuid = NULL) {

  $data = NULL;
  _deploy_extra_import_log($deployment_key, $uuid, $entity_type, $status, $data, $entity_vuuid);

}

/**
 * @TODO add the proper description
 */
function deploy_extra_import_log($deployment_key, $uuid, $entity_type, $data = array(), $entity_vuuid = NULL) {
  $status = DEPLOY_EXTRA_IMPORT_STATUS_PROCESSED;
  _deploy_extra_import_log($deployment_key, $uuid, $entity_type, $status, $data, $entity_vuuid);
}

/*
 * Structure errors Logging system.
 */
function deploy_extra_log_import_check($entity_type, $entity, $uuid, $config) {
  $deployment_key = $config['deployment_key'];
  $vuuid = deploy_extra_get_vuuid_from_entity($entity, $entity_type);

  // Creating a clone of the entity object to check,
  // that forbid directly changing the entity object.
  // $clone = clone $entity;
  $clone = $entity;

  // Make an entity local.
  entity_make_entity_local($entity_type, $clone);

  // Perform general checks for entity.
  $messages = deploy_extra_import_check_common($entity_type, $uuid, $clone, $deployment_key);

  // Checking structure.
  $messages += module_invoke_all('deploy_extra_check_structure', $entity_type, $clone);

  // Let other modules alter messages for this entity.
  drupal_alter('deploy_extra_check_structure', $messages, $entity_type, $clone);

  $data['message'] = $messages;

  // Write messages to log.
  deploy_extra_import_log($deployment_key, $uuid, $entity_type, $data, $vuuid);
}

/*
 * Perform general checks for entity.
 * These checks do not depend on the type of entity.
 */
function deploy_extra_import_check_common($entity_type, $uuid, $entity, $deployment_key) {
  $context = array('context' => DEPLOY_EXTRA_LOG_TRANSLATE_CONTEXT);
  $messages = array();

  // Define message type.
  $message_type = t('General', array(), $context);

  // Obtain a list of available entity types on the site.
  $entities = entity_get_info();

  // If this entity type does not exist on the site.
  // This situation isn't possible if you use the rest service,
  // but obviously possible for import from file/form.
  if (empty($entities[$entity_type])) {
    $messages[] = array(
      'message' => t('This entity type - %entity_type does not exist on the site.',
        array('%entity_type' => $entity_type), $context),
      'type' => $message_type,
    );
  }

  // If the entity type doesn't support UUIDs.
  if (empty($entities[$entity_type]['uuid'])) {
    $messages[] = array(
      'message' => t('This entity type - %entity_type does not support UUIDs.',
        array('%entity_type' => $entity_type), $context),
      'type' => $message_type,
    );
  }

  // Checking for the existence of an entity bundle.
  list(, , $bundle_name) = entity_extract_ids($entity_type, $entity);
  $entity_info = entity_get_info($entity_type);
  $bundles = $entity_info['bundles'];

  if (!isset($bundles[$bundle_name])) {
    $messages[] = array(
      'message' => t('Bundle - %bundle for the entity type - %entity does not exists.',
        array('%bundle' => $bundle_name, '%entity' => $entity_type), $context),
      'type' => $message_type,
    );
  }

  return $messages;
}

/**
 * Implements hook_deploy_extra_check_structure().
 */
function field_deploy_extra_check_structure($entity_type, $entity) {
  $messages = array();
  list(, , $bundle_name) = entity_extract_ids($entity_type, $entity);
  $instances = field_info_instances($entity_type, $bundle_name);

  foreach ($instances as $field_name => $instance) {
    $field = field_info_field($field_name);
    foreach ($entity->{$field_name} as $langcode => $items) {
      $field_errors_log = module_invoke($field['module'], 'deploy_extra_check_field_structure', $entity_type, $entity, $field, $instance, $langcode, $items);

      // Let other modules alter messages for the field.
      drupal_alter('deploy_extra_check_field_structure', $field_errors_log, $entity_type, $entity, $field, $instance, $langcode, $items);

      if (!empty($str_errors_log)) {
        $messages = array_merge_recursive($messages = array(), $field_errors_log);
      }
    }
  }
  return $messages;
}

/**
 * Implements hook_deploy_extra_check_structure().
 */
function node_deploy_extra_check_structure($entity_type, $entity) {
  if ($entity_type == 'node') {
    $context = array('context' => DEPLOY_EXTRA_LOG_TRANSLATE_CONTEXT);
    $messages = array();

    // Define the message type.
    $message_type = t('Node', array(), $context);

    // Getting all languages on the site.
    $lang = language_list();

    // Using the entity metadata wrapper to get the language property.
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $node_lang = $wrapper->language->value();

    // There is no such language on the site.
    if (!isset($lang[$node_lang]) && $node_lang != LANGUAGE_NONE) {
      $messages[] = array(
        'message' => t('No such language - %lang on the site.', array('%lang' => $node_lang), $context),
        'type' => $message_type,
      );
    }

    return $messages;
  }
}

/**
 * Implements hook_deploy_extra_check_field_structure().
 */
function taxonomy_field_deploy_extra_check_field_structure($entity_type, $entity, $field, $instance, $langcode, $items) {
  $context = array('context' => DEPLOY_EXTRA_LOG_TRANSLATE_CONTEXT);
  $messages = array();


  return $messages;
}

/**
 * Helper function.
 * Converts an array of log messages into a plain text.
 *
 * @param array $data
 * @return string
 */
function deploy_extra_log_message_to_text($data) {
  $text = '';
  foreach ($data as $row) {
    $text .= $row['type'] . ' : ' . $row['message'] . '<br>';
  }

  return $text;
}

/**
 * Helper function.
 * Converts the integer status to the human readable text.
 *
 * @param int $status
 *   The deploy process statuses - examples are 0, 1.
 *
 * @return
 *   Array of status/statuses of readable text or False.
 */
function _deploy_extra_log_status_to_string($status = NULL) {

  $statuses = array(
    DEPLOY_EXTRA_IMPORT_STATUS_PROCESSED => t('Processed'),
    DEPLOY_EXTRA_IMPORT_STATUS_SAVED => t('Saved'),
    DEPLOY_EXTRA_IMPORT_STATUS_UPDATED => t('Updated'),
    DEPLOY_EXTRA_IMPORT_STATUS_SKIPPED => t('Skipped'),
    DEPLOY_EXTRA_IMPORT_STATUS_TEST_MODE => t('Test Mode'),
    DEPLOY_EXTRA_IMPORT_STATUS_FAILED => t('Failed'),
  );

  if (!isset($status)) {
    return ($statuses);
  }

  if (array_key_exists($status, $statuses)) {
    return array($status => $statuses[$status]);
  }

  return FALSE;
}