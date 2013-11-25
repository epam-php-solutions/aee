<?php

/**
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

      // Let other modules have their say.
      drupal_alter('deploy_extra_export_entity', $entity);
    }
  }

  public function publish($deployment_key, DeployEndpoint $endpoint, $lock_name = NULL) {
    $url = 'admin/structure/deploy';
    $this->config['url'] = ($this->config['export_method'] == 'form' ? $url . '/export/' . $deployment_key : $url);

    $params = array(
      $deployment_key,
      $this->config['export_method'],
      $this->config['destination'],
      $this->aggregator->plan->name,
      $lock_name,
      $this->config['export_format'],
    );

    // Add to results params.
    $this->operations[]  = array('deploy_extra_iex_pass_deploy_info', $params);

    foreach ($this->data as $entity) {
      $this->operations[]  = array('deploy_extra_iex_prepare_data_entities', array($entity));
    }
  }

  public function postProcess(Array $operations = array()) {
    $this->processOperations($operations);
    $batch = array(
      'title' => t('Deploying %plan', array('%plan' => $this->aggregator->plan->title)),
      'operations' => $this->operations,
      'init_message' => t('Exporting Batch is starting.'),
      'progress_message' => t('Processed @current out of @total.'),
      'error_message' => t('Exporting Batch has encountered an error.'),
      'finished'   => 'deploy_extra_iex_export_batch_finished_operation',
    );
    batch_set($batch);
    batch_process($this->config['url']);
  }

  public function configForm(&$form_state) {
    // Use in translation context.
    $context = array('context' => DEPLOY_EXTRA_IEX_TRANSLATE_CONTEXT);

    // Get plugins.
    $formats = deploy_extra_iex_get_export_format_plugins();

    // Initialize the options array.
    $options = array();

    // Fill the options array.
    if (!empty($formats)) {
      foreach($formats as $format) {
        $options[$format['handler']['class']] = $format['name'];
      }
    }

    // Let other modules add their options.
    $methods = array();
    foreach (module_invoke_all('deploy_extra_iex_methods') as $method => $array) {
      if ($array['export_label'] != 'none') {
        $methods[$method] = $array['export_label'];
      }
    }

    // Let other modules change the methods options.
    drupal_alter('deploy_extra_iex_methods_alter', $methods);

    return array(
      'export_method' => array(
        '#type' => 'radios',
        '#title' => t('Choose the export method', array(), $context),
        '#options'  => $methods,
        '#required' => TRUE,
      ),
      'destination' => array(
        '#type' => 'textfield',
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
      ),
      'export_format' => array(
        '#type'  => 'radios',
        '#title' => t('Choose the export format', array(), $context),
        '#options'  => $options,
        '#required' => TRUE,
      ),
    );
  }
}
