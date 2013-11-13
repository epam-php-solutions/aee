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
    parent::__construct($config);

    // Let other modules have their say.
    drupal_alter(__CLASS__ . '_config', $config);

    $this->config = array_merge($this->config, $config);
  }

  public function deploy(Traversable $iterator) {

    foreach ($iterator as $entity) {

      // Let other modules have their say.
      drupal_alter(__CLASS__ . '_entity', $entity, $this->config);

      $json_data = drupal_json_encode($entity);
      $this->config['headers']['Content-Type'] = 'application/json';

      // TODO: Consider making the resource path configurable.
      $url = $this->config['url'] . '/' . $entity->__metadata['uri'] . '.json';
      // Temporary hack for the built-in PHP 5.4 web server that doesn't route
      // filenames correctly.
      // TODO: See if we can remove the .json suffix for all server. The
      // Content-Type header should be enough in most (all?) cases.
      if (php_sapi_name() == 'cli-server') {
        $url = $this->config['url'] . '/' . $entity->__metadata['uri'];
      }
      //$this->httpRequest($url, 'PUT', $json_data);
    }
  }

  public function publish(Traversable $iterator) {
    foreach ($iterator as $entity) {
      $this->config['headers']['Content-Type'] = 'application/json';
      // TODO: Try to get the last deploy structure log!
    }
  }

  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);

    // Let other modules have their say.
    drupal_alter(__CLASS__ . '_form', $form, $form_state, $this->config);

    return $form;
  }
}
