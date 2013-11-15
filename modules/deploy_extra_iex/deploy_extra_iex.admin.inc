<?php

/**
 * @file
 * Includes the admin callbacks and functions for Deploy Extra Import Export module.
 */

/**
 * @return mixed
 */
function deploy_extra_iex_entity_import_form() {
  // Fieldset.
  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import interface'),
  );

  // Modes of import.
  $form['import']['mode'] = array(
    '#type' => 'radios',
    '#title' => t('Choose mode'),
    '#options' => array(
      'form' => t('Import from form'),
      'file' => t('Import from file'),
    ),
    '#default_value' => 'form',
  );

  // Import textarea form.
  $form['import']['form'] = array(
    '#type'   => 'textarea',
    '#title'  => t('Paste you code here'),
    '#default_value' => '',
    '#states' => array(
      'enabled' => array(
        ':input[name="mode"]' => array(
          'value' => 'form',
        ),
      ),
      'visible' => array(
        ':input[name="mode"]' => array(
          'value' => 'form',
        ),
      ),
      'required' => array(
        ':input[name="mode"]' => array(
          'value' => 'form',
        ),
      ),
    ),
  );

  // File upload form.
  $form['import']['upload'] = array(
    '#type'   => 'managed_file',
    '#title'  => t('Upload deploy txt file'),
    '#descripion' => t('Only txt files are allowed.'),
    '#upload_location'   => 'public://deploy_import/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('txt'),
    ),
    '#states' => array(
      'enabled' => array(
        ':input[name="mode"]' => array(
          'value' => 'file',
        ),
      ),
      'visible' => array(
        ':input[name="mode"]' => array(
          'value' => 'file',
        ),
      ),
      'required' => array(
        ':input[name="mode"]' => array(
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
    '#value' => t('Import'),
  );

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function deploy_extra_iex_entity_import_form_submit($form, $form_state) {
  $values = $form_state['values'];

  // Get file ID.
  $fid = $values['upload'];

  // Load the file.
  $file = file_load($fid);

  // Initialize operations array.
  $operations = array();

  if (!$file && empty($values['form'])) {
    drupal_set_message("You can't to import empty data", 'error');
    return $form;
  }

  // Get the data from file or form.
  $data = $file ? file_get_contents($file->uri) : $values['form'];
  $data = drupal_json_decode($data);

  if (!empty($data)) {
    foreach($data as $entity) {
      $operations[] = array('_uuid_services_entity_update', array(
        $entity['__metadata']['type'],
        $entity['uuid'],
        $entity,
      ));
    }
  }

  $batch = array(
    'title' => t('Importing...'),
    'operations' => $operations,
    'finished'   => 'deploy_extra_iex_batch_finished_operation',
  );
  batch_set($batch);
  batch_process('admin/structure/deploy/import');
}

/**
 * Export form.
 */
function deploy_extra_iex_entity_export_form($form, $form_state, $uuid = NULL) {
  $cache = cache_get($uuid);
  cache_clear_all($uuid, 'cache');

  $form['export'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#default_value' => !empty($cache) ? drupal_json_encode($cache->data) : '',
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
  $cache = cache_get($uuid);
  cache_clear_all($uuid, 'cache');

  $filename = 'export_entities_data.txt';
  $content = drupal_json_encode($cache->data);

  // Send headers.
  header('Content-Description: File Transfer');
  header('Content-Type:text/plain; charset=utf-8');
  header('Content-Disposition: attachment; filename="' . $filename);
  header('Content-Length: '.strlen($content));

  // Send data.
  echo $content;
}