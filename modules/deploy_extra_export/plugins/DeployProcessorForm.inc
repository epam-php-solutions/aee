<?php

/**
 * @file
 * Contains DeployProcessorForm class.
 * Processor class using the Batch API.
 */
class DeployProcessorForm extends DeployProcessorMemory {
  /**
   * Batch operations to execute.
   *
   * @var array
   */
  public $operations = array();

  /**
   * Include entities data.
   *
   * @var array
   */
  public $data = array();

  public function deploy($deployment_key, DeployEndpoint $endpoint, $lock_name = NULL) {
    $iterator = $this->aggregator->getIterator();
    foreach ($iterator as $entity) {
      $this->data[] = $entity;

      // Add deployment key to entity metadata.
      if (isset($entity->__metadata)) {
        $entity->__metadata['deploy_extra']['deployment_key'] = $deployment_key;
      }

      // Also pass deployment key in config array and let other modules change entities if needed.
      $config['deployment_key'] = $deployment_key;
      drupal_alter('deploy_extra_export_entity', $entity, $config);
    }
  }

  public function publish($deployment_key, DeployEndpoint $endpoint, $lock_name = NULL) {
    $url = 'admin/structure/deploy';
    $this->config['url'] = ($this->config['export_method'] == 'form' ? $url . '/export/' . $deployment_key : $url);
    $this->config['lock_name'] = $lock_name;
    $this->config['plan_name'] = $this->aggregator->plan->name;

    // Initialize options array.
    $options = array(
      $deployment_key,
      $this->config,
    );

    // Add batch export operation for passing deploy info to results array.
    $this->operations[]  = array('deploy_extra_export_pass_deploy_info', $options);

    foreach ($this->data as $entity) {
      $this->operations[]  = array('deploy_extra_export_prepare_data_entities', array($entity));
    }
  }

  public function postProcess(Array $operations = array()) {
    $this->processOperations($operations);
    $batch = array(
      'title' => t('Deploying %plan', array('%plan' => $this->aggregator->plan->title)),
      'operations' => $this->operations,
      'init_message' => t('Exporting Batch is starting.'),
      'progress_message' => t('Processed @current out of @total.'),
      'error_message'    => t('Exporting Batch has encountered an error.'),
      'finished'         => 'deploy_extra_export_batch_finished_operation',
    );
    batch_set($batch);
    batch_process($this->config['url']);
  }

  public function configForm(&$form_state) {
    // Use in translation context.
    $context = array('context' => DEPLOY_EXTRA_EXPORT_TRANSLATE_CONTEXT);

    // Get export formats options.
    $options = deploy_extra_formats_get_formats_options($form_state, 'configExportForm');

    // Get export methods options.
    $methods = deploy_extra_formats_get_methods_options('export_label');

    // Let other modules change the methods options.
    drupal_alter('deploy_extra_formats_methods', $methods);

    // Export methods (form, file & etc.).
    $output['export_method'] = array(
      '#type' => 'radios',
      '#title' => t('Choose the export method', array(), $context),
      '#options' => $methods,
      '#required' => TRUE,
    );

    // Destination folder, when save the export file on server.
    $output['destination'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to folder', array(), $context),
      '#field_prefix' => 'default/files/',
      '#default_value' => isset($this->config['destination']) ? $this->config['destination'] : '',
      '#description' => t('The destination folder, where deploy files will located.', array(), $context),
      '#states' => array(
        'visible' => array(
          ':input[name="processor_config[export_method]"]' => array(
            'value' => 'file',
          )
        ),
      ),
    );

    // Export formats (JSON, YAML & etc.).
    $output['export_format'] = array(
      '#type' => 'radios',
      '#title' => t('Choose the export format', array(), $context),
      '#options' => $options['plugins_formats'],
      '#required' => TRUE,
    );

    // Add plugins config forms.
    $output['plugins_config'] = $options['plugins_forms'];

    return $output;
  }
}
