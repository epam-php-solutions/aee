<?php

/**
 * @file
 * Deploy Extra log module.
 */

/*
 * Structure errors Logging system.
 */
function deploy_extra_import_check($entity_type, $uuid, $entity, $deployment_key) {
  $messages = array();

  // Perform general checks for entity.
  $error = deploy_extra_import_check_common($entity_type, $uuid, $entity, $deployment_key);

  // Not necessary to perform other checks.
  if($error) return;

  // @TODO How to improve this?
  // Make an entity local.
  entity_make_entity_local($entity_type, $entity);

  // Obtain a list of available entity types on the site.
  $entities = entity_get_info();

  foreach ($entities as $entity_name => $entity_instance) {
    $errors_messages = module_invoke($entity_name, 'deploy_extra_check_structure', $entity_type, $entity);
    if (!empty($errors_messages)) {
      $messages = array_merge_recursive($messages = array(), $errors_messages);
    }
  }

  // Let other modules alter messages for this entity.
  drupal_alter('deploy_extra_check_structure', $messages, $entity_type, $entity);

  // Write messages to log.
  deploy_extra_import_log($deployment_key, $uuid, $messages);
}

/*
 * Perform general checks for entity.
 * These checks do not depend on the type of entity.
 */
function deploy_extra_import_check_common($entity_type, $uuid, $entity, $deployment_key) {
  $context  = array('context' => DEPLOY_EXTRA_LOG_TRANSLATE_CONTEXT);
  $messages = array();

  // Define message type.
  $message_type = t('Common', array(), $context);

  // Obtain a list of available entity types on the site.
  $entities = entity_get_info();

  // If this entity type does not exist on the site.
  if ($entities[$entity_type]) {
    $messages += array(
      'message' => t('This %entity_type does not exist on the site.', array('%entity_type' => $entity_type), $context),
      'type'    => $message_type,
    );
  }

  // If this entity type does not support UUIDs.
  if (empty($entities[$entity_type]['uuid'])) {
    $messages += array(
      'message' => t('This %entity_type does not support UUIDs.', array('%entity_type' => $entity_type), $context),
      'type'    => $message_type,
    );
  }

  // Check for the existence of an entity bundle.
  list(,, $bundle_name) = entity_extract_ids($entity_type, $entity);
  $entity_info = entity_get_info($entity_type);
  if(!in_array($bundle_name, $entity_info['bundles'])) {
    $messages += array(
      'message' => t('Bundle - %bundle does not exists.', array('%bundle' => $bundle_name), $context),
      'type'    => $message_type,
    );
  }

  // Write messages to log.
  deploy_extra_import_log($deployment_key, $uuid, $messages);

  return !empty($messages);
}

/**
 * Implements hook_deploy_extra_check_structure().
 */
function field_deploy_extra_check_structure($entity_type, $entity) {
  $messages = array();
  list(,, $bundle_name) = entity_extract_ids($entity_type, $entity);
  $instances = field_info_instances($entity_type, $bundle_name);

  foreach ($instances as $field_name => $instance) {
    $field = field_info_field($field_name);
    foreach ($entity->{$field_name} as $langcode => $items) {
      $field_errors_log = module_invoke($field['module'], 'deploy_extra_check_field_structure', $entity_type, $entity, $field, $instance, $langcode, $items);

      // Let other modules alter messages for this field.
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

    // Define message type.
    $message_type = t('Node', array(), $context);

    // All languages on the site.
    $lang = language_list();

    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $node_lang = $wrapper->language;

    // No such language on the site.
    if (!isset($lang[$node_lang])) {
      $messages += array(
        'message' => t('No such language - %lang on the site.', array('%lang' => $node_lang), $context),
        'type' => $message_type,
      );
    }

    return $messages;
  }
}