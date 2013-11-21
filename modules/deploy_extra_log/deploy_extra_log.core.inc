<?php

/**
 * @file
 * Deploy Extra log module.
 */

/**
 * Implements hook_deploy_extra_check_entity_structure().
 */
function field_deploy_extra_check_entity_structure($entity_type, $uuid, $entity) {
  $errors = array();
  list(,, $bundle_name) = entity_extract_ids($entity_type, $entity);
  $instances = field_info_instances($entity_type, $bundle_name);

  foreach ($instances as $field_name => $instance) {
    $field = field_info_field($field_name);
    foreach ($entity->{$field_name} as $langcode => $items) {
      $field_errors_log = module_invoke($field['module'], 'deploy_extra_check_field_structure', $entity_type, $entity, $field, $instance, $langcode, $items);

      // Let other modules alter dependencies for this field.
      drupal_alter('deploy_extra_check_field_structure', $field_errors_log, $entity_type, $entity, $field, $instance, $langcode, $items);

      if (!empty($str_errors_log)) {
        $errors = array_merge_recursive($errors, $field_errors_log);
      }
    }
  }
  return $errors;
}

