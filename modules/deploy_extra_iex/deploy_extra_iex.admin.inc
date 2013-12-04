<?php

/**
 * @file
 * Includes the admin callbacks and functions for Deploy Extra IEX module.
 */

/**
 * Interface form of import.
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

  // Get import formats options.
  $options = deploy_extra_iex_get_export_formats_options($form_state, 'configImportForm');

  // If deploy_extra_log is enable, show the additional settings.
  if (module_exists('deploy_extra_log')) {
    $form['import']['import_config'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Configuration of import', array(), $context),
      '#options' => array(
        'adv_log' => t('Use advanced log', array(), $context),
        'skip' => t('Skip the existing entity', array(), $context),
        'tmode' => t('Use test mode', array(), $context),
      ),
    );
  }

  // List of import formats.
  $form['import']['format'] = array(
    '#type' => 'radios',
    '#title' => t('Choose import formats', array(), $context),
    '#options' => $options['plugins_formats'],
    '#default_value' => 'DeployExtraJSON',
    '#required' => TRUE,
  );

  // Add config plugins forms.
  $form['import']['plugins_config'] = $options['plugins_forms'];

  // Get import methods options.
  $methods = deploy_extra_iex_get_methods_options('import_label');

  // List of import methods.
  $form['import']['method'] = array(
    '#type' => 'radios',
    '#title' => t('Choose mode', array(), $context),
    '#options' => $methods,
    '#default_value' => 'form',
    '#required' => TRUE,
  );

  // Import text area form.
  $form['import']['form'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste you code here', array(), $context),
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
    '#type' => 'textfield',
    '#title' => t('Path to txt file', array(), $context),
    '#field_prefix' => 'default/files/',
    '#description' => t('Please write the full path to file. Path must include file name, which you would like to import.', array(), $context),
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
    '#type' => 'managed_file',
    '#title' => t('Upload deploy txt file', array(), $context),
    '#description' => t('Only txt files are allowed.', array(), $context),
    '#upload_location' => 'public://deploy_import/',
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

  // Invoke all methods.
  $methods = module_invoke_all('deploy_extra_iex_methods');

  // Define the import callback.
  $function = isset($methods[$method]) ? $methods[$method]['import_callback'] : '';

  // Call the import callback.
  if (function_exists($function)) {
    $data = $function($values);
  }

  // If have empty data, set the error and reload the page.
  if (empty($data)) {
    drupal_set_message(t('You have empty data of entities', array(), $context), 'error');
    $form_state['rebuild'] = TRUE;
    return;
  }

  // Load export formatter and decode the data of entities.
  $formatter = deploy_extra_iex_get_export_formats_plugins($format);
  $plugin = new $formatter['handler']['class'];
  $data = $plugin->decodeData($data);
  $lock_name = !empty($data['processor_config']['lock_name']) ? $data['processor_config']['lock_name'] : NULL;

  // If have empty $lock_name, set the error and reload the page.
  if (!$lock_name) {
    drupal_set_message(t('You have incorrect data of entities', array(), $context), 'error');
    $form_state['rebuild'] = TRUE;
    return;
  }

  // We only allow one import process of each plan at the time.
  if (!lock_acquire($lock_name)) {
    throw new DeployPlanRunningException(t('The import of @lock_name is already running.', array('@lock_name' => $lock_name), $context));
  }

  // Pass data to submission function.
  $form_state['deploy_extra_iex_import_data'] = $data;
}

/**
 * Submission handler for deploy_extra_iex_entity_import_form().
 */
function deploy_extra_iex_entity_import_form_submit($form, $form_state) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Default value.
  $import_config = array();
  if (isset($form_state['values']['import_config'])) {
    foreach ($form_state['values']['import_config'] as $key => $value) {
      $import_config[$key] = !empty($value) ? 1 : 0;
    }
  }

  // Import data.
  $data = $form_state['deploy_extra_iex_import_data'];

  // Initialize operations array.
  $operations = array();

  if (!empty($data['entities'])) {
    foreach ($data['entities'] as $entity) {
      // Add deployment key to configuration array.
      $import_config['deployment_key'] = $entity['__metadata']['deploy_extra']['deployment_key'];

      // Array of input arguments for import operation.
      $options = array(
        $entity['__metadata']['type'],
        $entity['uuid'],
        $entity,
        $import_config,
      );

      // Add import operation and pass the arguments to it.
      $operations[] = array('deploy_extra_import_entity', $options);
    }
  }

  // Batch options.
  $batch = array(
    'title' => t('Importing...', array(), $context),
    'operations' => $operations,
    'finished' => 'deploy_extra_iex_import_batch_finished_operation',
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
 *
 * @param $filename
 *   Filename for downloading.
 */
function deploy_extra_iex_ajax_deploy_download($uuid, $filename) {
  $cache = deploy_extra_iex_get_cache($uuid);
  $content = $cache->data;

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
  $cid = 'deploy_extra_iex_' . $uuid;
  $cache = cache_get($cid);
  cache_clear_all($uuid, 'cache');

  return $cache;
}

/**
 * Form import callback.
 *
 * @param $values
 *   Values array after submission form.
 *
 * @return string
 *   Return encoded string.
 */
function deploy_extra_iex_form_import($values) {
  return $values['form'];
}

/**
 * File for downloading import callback.
 *
 * @param $values
 *   Values array after submission form.
 *
 * @return string
 *   Return encoded string.
 */
function deploy_extra_iex_file_download_import($values) {
  $path = $values['file_path'];
  $uri = 'public://' . $path;
  return file_exists($uri) ? file_get_contents($uri) : '';
}

/**
 * File import callback.
 *
 * @param $values
 *   Values array after submission form.
 *
 * @return string
 *   Return encoded string.
 */
function deploy_extra_iex_file_import($values) {
  $fid = $values['upload'];
  return file_get_contents(file_load($fid)->uri);
}
