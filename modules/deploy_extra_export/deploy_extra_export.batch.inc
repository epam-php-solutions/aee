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
function deploy_extra_export_batch_finished_operation($success, $results, $operations) {
  $uuid = $results['uuid'];
  $processor_config = $results['processor_config'];
  $lock_name = $processor_config['lock_name'];

  try {
    $export_method = $processor_config['export_method'];
    $export_format = $processor_config['export_format'];

    // Load export formatter and encode the data of entities.
    $formatter = deploy_extra_formats_get_formats_plugins($export_format);
    $class = new $formatter['handler']['class'];
    $data  = $class->encodeData(
      array(
        'processor_config' => $processor_config,
        'entities'  => $results['entities'],
      )
    );

    // Put encoded data in cache.
    cache_set('deploy_extra_export_' . $uuid, $data, 'cache', CACHE_TEMPORARY);

    // Invoke all methods.
    $methods = module_invoke_all('deploy_extra_formats_methods');

    // If exist method, call the export callback.
    if (isset($methods[$export_method])) {
      $results['filename'] = $processor_config['plan_name'] . '_' . date('d-m-Y(H.i)') . '.txt';
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
 * Batch export operation for add entity to results array.
 */
function deploy_extra_export_prepare_data_entities($entity, &$context = NULL) {
  $context['results']['entities'][] = $entity;
}

/**
 * Batch export operation for passing deploy info to results array.
 *
 * @param $uuid
 *   Unique uid (deployment key).
 *
 * @param $processor_config
 *   Configuration array of processor.
 *
 * @param null $context
 */
function deploy_extra_export_pass_deploy_info($uuid, $processor_config, &$context = NULL) {
  $context['results']['uuid'] = $uuid;
  $context['results']['processor_config'] = $processor_config;
}
