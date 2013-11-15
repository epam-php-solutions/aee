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

  protected function processOperations(Array $operations = array()) {
    if (!empty($operations)) {
      foreach ($this->aggregator as $entity) {
        foreach ($operations as $operation) {
          $this->operations[] = array($operation['callback'], array($this->aggregator->plan->name, $entity));
        }
      }
    }
  }

  public function deploy($deployment_key, DeployEndpoint $endpoint, $lock_name = NULL) {
    $iterator = $this->aggregator->getIterator();
    foreach ($iterator as $entity) {
      $this->data[] = $entity;
    }

    // Save deployment_key in config array.
    $this->config['uuid'] = $deployment_key;
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
    );

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
      'finished'   => 'deploy_extra_iex_batch_finished_operation',
    );
    batch_set($batch);
    batch_process($this->config['url']);
  }

  public function configForm(&$form_state) {
    return array(
      'export_method' => array(
        '#type' => 'radios',
        '#options' => array(
          'form' => t('Push to form'),
          'file' => t('Push to browser for downloading'),
          'path' => t('Push to destination on server folder'),
        ),
        '#default_value' => 'form',
      ),
      'destination' => array(
        '#type' => 'textfield',
        '#field_prefix' => 'default/files/',
        '#default_value' => isset($this->config['destination']) ? $this->config['destination'] : '',
        '#description' => t('The destination folder, where deploy files will located.'),
        '#states' => array(
          'visible' => array(
            ':input[name="processor_config[export_method]"]' => array(
              'value' => 'path',
            )
          ),
        ),
      ),
    );
  }
}
