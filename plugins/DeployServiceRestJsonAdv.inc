<?php

/**
 * Class for deploying to a REST server using JSON
 * TODO add description
 */
class DeployServiceRestJsonAdv extends DeployServiceRest {

  /**
   * Constructor for a service plugin.
   *
   * @param array $config
   *   An associative array of configuration settings for the service plugin.
   */
  public function __construct(Array $config = array()) {
    // Add custom options to default config.
    $default_config = $this->defaultConfig();
    $this->config += $default_config;

    parent::__construct($config);
  }

  public function deploy(Traversable $iterator) {

    foreach ($iterator as $entity) {

      // Add config to entity __metadata.
      $this->fillEntityMetadata($entity);

      // Let other modules have their say.
      drupal_alter('deploy_extra_export_entity', $entity, $this->config);

      $json_data = drupal_json_encode($entity);
      $this->config['headers']['Content-Type'] = 'application/json';

      // See comments in the DeployServiceRestJson::deploy method.
      $url = $this->config['url'] . '/' . $entity->__metadata['uri'] . '.json';
      if (php_sapi_name() == 'cli-server') {
        $url = $this->config['url'] . '/' . $entity->__metadata['uri'];
      }
      $this->httpRequest($url, 'PUT', $json_data);
    }
  }

  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    $config = $this->config;

    // Settings for using test mode.
    $form['tmode'] = array(
      '#type' => 'checkbox',
      '#title' => t('Test mode'),
      '#description' => t('Check this to enable test mode without creating or updating entities on the endpoint side.'),
      '#default_value' => $config['tmode'],
      '#weight' => 8,
    );

    // Settings to skip update operation.
    $form['skip'] = array(
      '#type' => 'checkbox',
      '#title' => t('Skip update'),
      '#description' => t('Check this to enable the skip mode. If the entity already exists update operation will be skipped.'),
      '#default_value' => $config['skip'],
      '#weight' => 8,
    );

    // Let other modules have their say.
    drupal_alter(__CLASS__ . '_form', $form, $form_state, $this->config);

    return $form;
  }

  public function fillEntityMetadata ($entity) {
    if (isset($entity->__metadata)) {
      $metadata = &$entity->__metadata;
      $config = $this->config;

      // Add in the Metadata only those properties
      // that are defined in the defaultConfig() method.
      $to_metadata = $this->defaultConfig();
      foreach ($to_metadata as $property => $value) {
        if(isset($config[$property])) {
          $metadata['deploy_extra'][$property] = $config[$property];
        }
      }
    }
  }

  public function defaultConfig() {

    // Add default values for tmode and skip.
    $config = array(
      'tmode' => FALSE,
      'skip' => FALSE,
      'deployment_key' => FALSE,
    );

    // Let other modules have their say.
    drupal_alter(__CLASS__ . '_config', $config);

    return $config;
  }
}
