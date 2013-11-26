<?php

/**
 * @file
 * This is the file description for Deploy Extra IEX batch.
 */

/**
 * Batch finish export operation.
 *
 * @param $success
 *   Status of batch.
 *
 * @param $results
 *   Batch results
 *
 * @param $operations
 *   Batch operations.
 *
 * @throws Exception
 */
function deploy_extra_iex_export_batch_finished_operation($success, $results, $operations) {
  $uuid = $results['uuid'];
  $lock_name = $results['lock_name'];

  try {
    $export_method = $results['export_method'];
    $export_format = $results['export_format'];

    // Load export formatter and encode the data of entities.
    $formatter = deploy_extra_iex_get_export_formats_plugins($export_format);
    $class = new $formatter['handler']['class'];
    $data  = $class->encodeData(
      array(
        'lock_name' => $results['lock_name'],
        'entities'  => $results['entities'],
      )
    );

    // Put encoded data in cache.
    cache_set('deploy_extra_iex_' . $uuid, $data, 'cache', CACHE_TEMPORARY);

    // Invoke all methods.
    $methods = module_invoke_all('deploy_extra_iex_methods');

    if (isset($methods[$export_method])) {
      $function = $methods[$export_method]['export_callback'];
      $function($success, $results);
    }
  }
  catch (Exception $e) {
    if (!empty($lock_name)) {
      lock_release($lock_name);
    }
    deploy_log($uuid, DEPLOY_STATUS_FAILED, $e);
    throw $e;
  }
}

/**
 * Batch finish import operation.
 *
 * @param $success
 *   Status of batch.
 *
 * @param $results
 *   Batch results
 *
 * @param $operations
 *   Batch operations.
 */
function deploy_extra_iex_import_batch_finished_operation($success, $results, $operations) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  $status  = $success ? 'status' : 'error';
  $message = $success ? 'Import of entities has done successfully!' : 'Import of entities has failed';

  // Send the message.
  drupal_set_message(t($message, array(), $context), $status);
}

/**
 * Batch export operation for add entity to results array.
 */
function deploy_extra_iex_prepare_data_entities($entity, &$context = NULL) {
  $context['results']['entities'][] = $entity;
}

/**
 * Batch export operation for passing deploy info to results array.
 *
 * @param $uuid
 *   Unique uid (deployment key).
 *
 * @param $export_method
 *   Method of export (file, path, form).
 *
 * @param $destination
 *   Destination folder for saving file, if choose the path method.
 *
 * @param $plan_name
 *   Deploy plan name.
 *
 * @param $lock_name
 *   The lock name.
 *
 * @param $export_format
 *   Export format (json, yaml & etc.).
 *
 * @param null $context
 *   Array passed from operations.
 */
function deploy_extra_iex_pass_deploy_info($uuid, $export_method, $destination, $plan_name, $lock_name, $export_format, &$context = NULL) {
  $context['results']['uuid'] = $uuid;
  $context['results']['export_method'] = $export_method;
  $context['results']['export_format'] = $export_format;
  $context['results']['destination']   = $destination;
  $context['results']['plan_name'] = $plan_name;
  $context['results']['lock_name'] = $lock_name;
}
