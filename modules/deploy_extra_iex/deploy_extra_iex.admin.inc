<?php

/**
 * @file
 * Includes the admin callbacks and functions for Deploy Extra Import Export module.
 */

/**
 * Import form.
 *
 * @see deploy_extra_iex_entity_import_form_submit()
 *
 * @return mixed
 */
function deploy_extra_iex_entity_import_form() {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Fieldset.
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import interface', array(), $context),
  );

  // Get plugins.
  $formats = deploy_extra_iex_get_export_format_plugins();
  $options = array();
  if (!empty($formats)) {
    foreach($formats as $format) {
      $options[$format['handler']['class']] = $format['name'];
    }
  }

  // Config of import.
  $form['import']['config'] = array(
    '#type'  => 'checkboxes',
    '#title' => t('Configuration of import', array(), $context),
    '#options' => array(
      'adv_log' => t('Use advanced log', array(), $context),
      'skip'    => t('Skip the existing entity', array(), $context),
      'tmode'   => t('Use test mode', array(), $context),
    ),
  );

  // Formats of import.
  $form['import']['format'] = array(
    '#type'  => 'radios',
    '#title' => t('Choose import formats', array(), $context),
    '#options' => $options,
    '#default_value' => 'form',
    '#required' => TRUE,
  );

  $methods = array();
  foreach (module_invoke_all('deploy_extra_iex_methods') as $method => $array) {
    if ($array['import_label'] != 'none') {
      $methods[$method] = $array['import_label'];
    }
  }

  // Methods of import.
  $form['import']['method'] = array(
    '#type'  => 'radios',
    '#title' => t('Choose mode', array(), $context),
    '#options' => $methods,
    '#default_value' => 'form',
    '#required' => TRUE,
  );

  // Import textarea form.
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

  // File upload form.
  $form['import']['upload'] = array(
    '#type'   => 'managed_file',
    '#title'  => t('Upload deploy txt file', array(), $context),
    '#descripion' => t('Only txt files are allowed.'),
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
 * Submission handler for deploy_extra_iex_entity_import_form().
 */
function deploy_extra_iex_entity_import_form_submit($form, $form_state) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

  // Get values.
  $values = $form_state['values'];
  $format = $values['format'];
  $config = $values['config'];

  // Get file ID.
  $fid = $values['upload'];

  // Load the file.
  $file = file_load($fid);

  // Initialize operations array.
  $operations = array();

  if (!$file && empty($values['form'])) {
    drupal_set_message(t("You can't to import empty data", array(), $context), 'error');
    return $form;
  }

  // Get the data from file or form.
  $data = $file ? file_get_contents($file->uri) : $values['form'];

  // Load export formatter and decode the data of entities.
  $formatter = deploy_extra_iex_get_export_format_plugins($format);
  $class = new $formatter['handler']['class'];
  $data  = $class->decodeData($data);

  if (!empty($data)) {
    foreach($data as $entity) {
      $operations[] = array('deloy_extra_import_entity', array(
        $entity['__metadata']['type'],
        $entity['uuid'],
        $entity,
        $config,
      ));
    }
  }

  $batch = array(
    'title' => t('Importing...'),
    'operations' => $operations,
    'finished'   => 'deploy_extra_iex_import_batch_finished_operation',
  );
  batch_set($batch);
  batch_process('admin/structure/deploy/import');
}

/**
 * Export form.
 *   Get data from cache and put them  to form.
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
 *   Deployment key (unique uid).
 */
function deploy_extra_iex_ajax_deploy_download($uuid) {
  $cache = deploy_extra_iex_get_cache($uuid);
  $filename = 'export_entities_data.txt';
  $content  = drupal_json_encode($cache->data['entities']);

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
