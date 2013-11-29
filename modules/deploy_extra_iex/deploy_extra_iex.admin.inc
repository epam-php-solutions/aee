<?php

/**
 * @file
 * Includes the admin callbacks and functions for Deploy Extra IEX module.
 */

/**
 * Import form.
 *
 * @param $form
 * @param $form_state
 *
 * @see deploy_extra_iex_entity_import_form_validate()
 * @see deploy_extra_iex_entity_import_form_submit()
 *
 * @return mixed
 */
function deploy_extra_iex_entity_import_form($form, &$form_state) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Fieldset.
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import interface', array(), $context),
  );

  // Get plugins.
  $formats = deploy_extra_iex_get_export_formats_plugins();
  $options = array();
  $plugins_forms = array();

  if (!empty($formats)) {
    foreach($formats as $format) {
      // Add format name to options array.
      $options[$format['handler']['class']] = $format['name'];

      // Initialize plugin format.
      $plugin_format = new $format['handler']['class'];

      // Get plugin config form.
      $plugins_forms['config'] = $plugin_format->configImportForm($form_state);
    }
  }

  // If deploy_extra_log is enable, show the additional settings.
  if (module_exists('deploy_extra_log')) {
    // Config of import.
    $form['import']['import_config'] = array(
      '#type'  => 'checkboxes',
      '#title' => t('Configuration of import', array(), $context),
      '#options' => array(
        'adv_log' => t('Use advanced log', array(), $context),
        'skip'    => t('Skip the existing entity', array(), $context),
        'tmode'   => t('Use test mode', array(), $context),
      ),
    );
  }

  // List of import formats.
  $form['import']['format'] = array(
    '#type'  => 'radios',
    '#title' => t('Choose import formats', array(), $context),
    '#options' => $options,
    '#default_value' => 'DeployExtraJSON',
    '#required' => TRUE,
  );

  // Add config plugins forms.
  $form['import']['plugins_config'] = $plugins_forms;

  // Get all import methods.
  $methods = array();
  foreach (module_invoke_all('deploy_extra_iex_methods') as $method => $array) {
    if ($array['import_label'] != 'none') {
      $methods[$method] = $array['import_label'];
    }
  }

  // List of import methods.
  $form['import']['method'] = array(
    '#type'  => 'radios',
    '#title' => t('Choose mode', array(), $context),
    '#options' => $methods,
    '#default_value' => 'form',
    '#required' => TRUE,
  );

  // Import text area form.
  $form['import']['form'] = array(
    '#type'   => 'textarea',
    '#title'  => t('Paste you code here', array(), $context),
    '#default_value' => '',
    '#states' => array(
      'enabled' => array(
        ':input[name="method"]' => array(
          'value' => 'form',
        ),
      ),
      'visible' => array(
        ':input[name="method"]' => array(
          'value' => 'form',
        ),
      ),
      'required' => array(
        ':input[name="method"]' => array(
          'value' => 'form',
        ),
      ),
    ),
  );

  // Path to file in directory.
  $form['import']['file_path'] = array(
    '#type'   => 'textfield',
    '#title'  => t('Path to txt file', array(), $context),
    '#field_prefix' => 'default/files/',
    '#description' => t('Please write the full path to file. Path must include file name, which you would like to import.'),
    '#states' => array(
      'enabled' => array(
        ':input[name="method"]' => array(
          'value' => 'file_for_downloading',
        ),
      ),
      'visible' => array(
        ':input[name="method"]' => array(
          'value' => 'file_for_downloading',
        ),
      ),
      'required' => array(
        ':input[name="method"]' => array(
          'value' => 'file_for_downloading',
        ),
      ),
    ),
  );

  // File upload form.
  $form['import']['upload'] = array(
    '#type'   => 'managed_file',
    '#title'  => t('Upload deploy txt file', array(), $context),
    '#description' => t('Only txt files are allowed.', array(), $context),
    '#upload_location'   => 'public://deploy_import/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('txt'),
    ),
    '#states' => array(
      'enabled' => array(
        ':input[name="method"]' => array(
          'value' => 'file',
        ),
      ),
      'visible' => array(
        ':input[name="method"]' => array(
          'value' => 'file',
        ),
      ),
      'required' => array(
        ':input[name="method"]' => array(
          'value' => 'file',
        ),
      ),
    ),
  );

  // Wrapper for action buttons.
  $form['import']['actions'] = array(
    '#type' => 'actions',
  );

  // Submit button.
  $form['import']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import', array(), $context),
  );

  return $form;
}

/**
 * Validation handler for deploy_extra_iex_entity_import_form().
 *
 * @throws DeployPlanRunningException
 */
function deploy_extra_iex_entity_import_form_validate($form, &$form_state) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Get values.
  $values = $form_state['values'];
  $format = $values['format'];
  $method = $values['method'];
  $data = '';

  switch ($method) {
    case 'file_for_downloading':
      $path = $values['file_path'];
      $uri = 'public://' . $path;

      if (file_exists($uri)) {
        $data = file_get_contents($uri);
      }
      break;

    case 'form':
      $data = $values['form'];
      break;

    case 'file':
      // Get file ID.
      $fid = $values['upload'];
      // Load the file.
      $file = file_load($fid);
      // Get the data from file or form.
      $data = file_get_contents($file->uri);
      break;
  }

  if (empty($data)) {
    drupal_set_message(t("You have empty data of entities", array(), $context), 'error');
    $form_state['rebuild'] = TRUE;
    return;
  }

  // Load export formatter and decode the data of entities.
  $formatter = deploy_extra_iex_get_export_formats_plugins($format);
  $class   = new $formatter['handler']['class'];
  $decoded = $class->decodeData($data);

  if (!isset($decoded['processor_config']['lock_name'])) {
    drupal_set_message(t("You have incorrect data of entities", array(), $context), 'error');
    $form_state['rebuild'] = TRUE;
    return;
  }

  // We only allow one import process of each plan at the time.
  $lock_name = $decoded['processor_config']['lock_name'];
  if (!lock_acquire($lock_name)) {
    throw new DeployPlanRunningException(t('The import of @lock_name is already running.', array('@lock_name' => $lock_name), $context));
  }

  // Pass data to submission function.
  $form_state['deploy_extra_iex_data'] = $decoded;
}

/**
 * Submission handler for deploy_extra_iex_entity_import_form().
 */
function deploy_extra_iex_entity_import_form_submit($form, $form_state) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Default value.
  $import_config = array();
  if (isset($values['import_config'])) {
    $import_config = $values['import_config'];
  }

  // Decoded data.
  $data = $form_state['deploy_extra_iex_data'];

  // Initialize operations array.
  $operations = array();

  if (!empty($data['entities'])) {
    foreach($data['entities'] as $entity) {
      $import_config['deployment_key'] = $entity['__metadata']['deploy_extra']['deployment_key'];
      $operations[] = array('deloy_extra_import_entity', array(
        $entity['__metadata']['type'],
        $entity['uuid'],
        (object)$entity,
        $import_config,
      ));
    }
  }

  $batch = array(
    'title' => t('Importing...', array(), $context),
    'operations' => $operations,
    'finished'   => 'deploy_extra_iex_import_batch_finished_operation',
  );
  batch_set($batch);
  batch_process('admin/structure/deploy/import');
}

/**
 * Export form.
 *   Get data from cache and put them to form.
 */
function deploy_extra_iex_entity_export_form($form, $form_state, $uuid = NULL) {
  $cache = deploy_extra_iex_get_cache($uuid);

  $form['export'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#default_value' => !empty($cache->data) ? $cache->data : '',
  );

  return $form;
}

/**
 * Ajax callback for output the data for downloading.
 *
 * @param $uuid
 *   Deployment key.
 */
function deploy_extra_iex_ajax_deploy_download($uuid) {
  $cache = deploy_extra_iex_get_cache($uuid);
  $content  = $cache->data;
  $filename = 'export_entities_data.txt';

  // Send headers.
  header('Content-Description: File Transfer');
  header('Content-Type:text/plain; charset=utf-8');
  header('Content-Disposition: attachment; filename="' . $filename);
  header('Content-Length: ' . strlen($content));

  // Send data.
  echo $content;
}

/**
 * Gets cache data of entities.
 *   Returns cache data and clears the cache data in DB.
 *
 * @param $uuid
 * @return mixed
 */
function deploy_extra_iex_get_cache($uuid) {
  $cid   = 'deploy_extra_iex_' . $uuid;
  $cache = cache_get($cid);
  cache_clear_all($uuid, 'cache');

  return $cache;
}
