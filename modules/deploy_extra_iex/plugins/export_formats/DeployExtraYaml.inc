<?php

class DeployExtraYaml implements DeployExtraFormat {

  public function __construct($config = array()) {
    return $config;
  }

  public function encodeData($data) {
    $this->loadSpyc();
    foreach($data['entities'] as &$entity) {
      $entity = (array)$entity;
    }
    unset($entity);
    return Spyc::YAMLDump($data);
  }

  public function decodeData($data) {
    $this->loadSpyc();
    return Spyc::YAMLLoad($data);
  }

  /**
   * Load the spyc library.
   */
  private function loadSpyc() {
    libraries_load('spyc');
  }
}
