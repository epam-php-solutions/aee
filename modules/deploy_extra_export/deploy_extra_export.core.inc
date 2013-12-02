<?php

/**
 * @file
 * This is the file description for Deploy Extra IEX core.
 */

/**
 * Add states behaviors to deploy_extra_export_endpoint checkbox.
 */
function deploy_extra_export_endpoints_after_build($element, &$form_state) {
  if (isset($element['deploy_extra_export_endpoint'])) {
    $element['deploy_extra_export_endpoint']['#states'] = array(
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
function deploy_extra_export_validate_endpoints($element, &$form_state) {
  $values = $form_state['values'];
  $plugin = isset($values['processor_plugin']) ? $values['processor_plugin'] : NULL;

  if ($plugin && $plugin == 'DeployProcessorForm' && empty($element['#value'])) {
    // Use in translation context.
    $context = array('context' => DEPLOY_EXTRA_EXPORT_TRANSLATE_CONTEXT);
    form_error($element, t('For use Processor Form you must select at least one endpoint', array(), $context));
  }
}

/**
 * Form export callback.
 *
 * @param $success
 *    Success or fail of batch operations.
 *
 * @param $results
 *    Array includes results and processor config.
 */
function deploy_extra_export_form_export($success, $results) {
  $uuid  = $results['uuid'];
  // Set message.
  deploy_extra_export_return_message($uuid, $success);
  drupal_goto('admin/structure/deploy/export/' . $uuid);
}

/**
 * File export callback.
 *   Save the encoded data of entities to file.
 *
 * @param $success
 *    Success or fail of batch operations.
 *
 * @param $results
 *    Array includes results and processor config.
 */
function deploy_extra_export_file_export($success, $results) {
  $uuid  = $results['uuid'];
  $cid   = 'deploy_extra_export_' . $uuid;
  $cache = cache_get($cid);

  $config = $results['processor_config'];
  $destination = trim($config['destination']);
  $dir  = 'public://' . $destination . '/';

  // Check if directory is exist, if not - create it.
  if (!is_dir($dir)) {
    mkdir($dir);
  }

  $filename = $results['filename'];
  $file = fopen($dir . $filename, 'a+');
  fwrite($file, $cache->data);

  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_EXPORT_TRANSLATE_CONTEXT);
  $suffix = t('File name is !file', array('!file' => l($filename, 'ajax/deploy/download/' . $uuid . '/' . $filename)), $context);

  // Set message.
  deploy_extra_export_return_message($uuid, $success, $suffix);
}

/**
 * File for downloading export callback.
 *
 * @param $success
 *    Success or fail of batch operations.
 *
 * @param $results
 *    Array includes results and processor config.
 */
function deploy_extra_export_file_download_export($success, $results) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_EXPORT_TRANSLATE_CONTEXT);

  $uuid = $results['uuid'];
  $filename = $results['filename'];
  $file_link = l($filename, 'ajax/deploy/download/' . $uuid . '/' . $filename);
  $suffix = t('You can download it here !file', array('!file' => $file_link), $context);

  // Set message.
  deploy_extra_export_return_message($uuid, $success, $suffix);
}

/**
 * Return the message about status of export.
 */
function deploy_extra_export_return_message($uuid, $success, $suffix = NULL) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_EXPORT_TRANSLATE_CONTEXT);

  // Default values.
  $mess_status = 'error';
  $dlog_status = DEPLOY_STATUS_FAILED;
  $message = 'Export of entities has failed';

  // Override the values if result is success.
  if ($success) {
    $mess_status = 'status';
    $dlog_status = DEPLOY_STATUS_PUBLISHED;
    $message = 'Export of entities has done successfully!' . ($suffix ? ' ' . $suffix : '');
  }

  deploy_log($uuid, $dlog_status);
  drupal_set_message(t($message, array(), $context), $mess_status);
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
