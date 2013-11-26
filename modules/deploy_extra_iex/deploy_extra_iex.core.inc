<?php

/**
 * @file
 * This is the file description for Deploy Extra IEX core.
 */

/**
 * Add states behaviors to deploy_extra_iex_endpoint checkbox.
 */
function deploy_extra_endpoints_after_build($element, &$form_state) {
  if (isset($element['deploy_extra_iex_endpoint'])) {
    $element['deploy_extra_iex_endpoint']['#states'] = array(
      'checked' => array(
        ':input[name="processor_plugin"]' => array(
          'value' => 'DeployProcessorForm',
        )
      ),
      'enabled' => array(
        ':input[name="processor_plugin"]' => array(
          'value' => 'DeployProcessorForm',
        )
      ),
    );
  }

  return $element;
}

/**
 * Validation handler for table of endpoints.
 */
function deploy_extra_validate_endpoints($element, &$form_state) {
  $values = $form_state['values'];
  $condition = isset($values['processor_plugin']) && $values['processor_plugin'] == 'DeployProcessorForm';
  if ($condition && empty($element['#value'])) {
    // Use in translation context.
    $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);
    form_error($element, t('For use Processor Form you must select at least one endpoint', array(), $context));
  }
}

/**
 * Return plugins configuration.
 *
 * @param null $id
 *   Format ID (DeployExtraJSON, DeployExtraYaml & etc.)
 *
 * @return array
 */
function deploy_extra_iex_get_export_formats_plugins($id = NULL) {
  ctools_include('plugins');
  return ctools_get_plugins('deploy_extra_iex', 'export_formats', $id);
}

/**
 * Form export callback.
 *
 * @param $success
 * @param $results
 */
function deploy_extra_iex_form_export($success, $results) {
  $uuid  = $results['uuid'];
  // Set message.
  deploy_extra_iex_return_message($uuid, $success);
  drupal_goto('admin/structure/deploy/export/' . $uuid);
}

/**
 * File export callback.
 *   Save the encoded data of entities to file.
 *
 * @param $success
 * @param $results
 */
function deploy_extra_iex_file_export($success, $results) {
  $uuid  = $results['uuid'];
  $cid   = 'deploy_extra_iex_' . $uuid;
  $cache = cache_get($cid);

  $destination = trim($results['destination']);
  $dir  = 'public://' . $destination . '/';

  // Check if directory is exist.
  if (!is_dir($dir)) {
    mkdir($dir);
  }

  $filename = $results['plan_name'] . '_' . date("d-m-Y(H.i)") . '.txt';
  $file = fopen($dir . $filename, 'a+');
  fwrite($file, $cache->data);

  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);
  $suffix = t('File name is !file', array('!file' => l($filename, 'ajax/deploy/download/' . $uuid)), $context);
  deploy_extra_iex_return_message($uuid, $success, $suffix);
}

/**
 * File for downloading export callback.
 *
 * @param $success
 * @param $results
 */
function deploy_extra_iex_file_download_export($success, $results) {
  $uuid  = $results['uuid'];
  $file_link = l('export_entities_data.txt', 'ajax/deploy/download/' . $uuid);
  $suffix = t('You can download it here !file', array('!file' => $file_link));
  // Set message.
  deploy_extra_iex_return_message($uuid, $success, $suffix);
}

/**
 * Return the message about status of export.
 *
 * @param $uuid
 * @param $success
 * @param null $suffix
 */
function deploy_extra_iex_return_message($uuid, $success, $suffix = NULL) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Default values.
  $status  = 'error';
  $message = 'Export of entities has failed';

  // Override the values if result is success.
  if ($success) {
    $status  = 'status';
    $message = 'Export of entities has done successfully!' . ($suffix ? ' ' . $suffix : '');
  }

  deploy_log($uuid, DEPLOY_STATUS_PUBLISHED);
  drupal_set_message(t($message, array(), $context), $status);
}

/**
 * Load function for uuid.
 *   Check that uuid is correct.
 *
 * @param null $uuid
 *   Unique UID.
 *
 * @return bool|null
 */
function uuid_load($uuid = NULL) {
  if ($uuid) {
    $query = db_select('deploy_deployments', 'dd')
      ->fields('dd', array('did'))
      ->condition('dd.uuid', $uuid)
      ->execute()->fetchField();
  }

  return !empty($query) ? $uuid : FALSE;
}
