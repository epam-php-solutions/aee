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
  }

  public function publish($deployment_key, DeployEndpoint $endpoint, $lock_name = NULL) {
    switch ($this->config['export_method']) {
      case 'form':
        $this->operations[] = array('deploy_extra_iex_form_output', array($this->data));
        break;

      case 'file':
        $data = drupal_json_encode($this->data);
        $this->operations[] = array('deploy_extra_iex_file_output', array($data));
        break;

      case 'path':
        try {
          $path = trim($this->config['destination']);
          $dir  = 'public://' . $path . '/';

          if (!is_dir($dir)) {
            mkdir($dir);
          }

          $filename = $this->aggregator->plan->name . '_' . date("d-m-Y(H.i)") . '.txt';
          $file = fopen($dir . $filename, 'a+');
          fwrite($file, drupal_json_encode($this->data));
          deploy_log($deployment_key, DEPLOY_STATUS_PUBLISHED);
        }
        catch (Exception $e) {
          if (!empty($lock_name)) {
            lock_release($lock_name);
          }
          deploy_log($deployment_key, DEPLOY_STATUS_FAILED, $e);
          throw $e;
        }
        break;
    }
  }

  public function postProcess(Array $operations = array()) {
    $this->processOperations($operations);
    $batch = array(
      'title' => t('Deploying %plan', array('%plan' => $this->aggregator->plan->title)),
      'operations' => $this->operations,
      'finished' => 'deploy_extra_iex_batch_finished_operation',
    );
    batch_set($batch);
    batch_process('admin/structure/deploy');
  }

  /**
   * @param array $form_state
   * @return array
   */
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
        '#description' => t('The destination, where deploy files are located.'),
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
