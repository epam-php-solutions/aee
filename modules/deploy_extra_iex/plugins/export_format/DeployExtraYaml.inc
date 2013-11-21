<?php

// if library is not exist disable the radio button (libraries_detect()).
class DeployExtraYaml implements DeployExtraExportFormat {

  public function __construct($config = array()) {
    return $config;
  }

  public function encodeData($data) {
    $this->loadSpyc();
    foreach($data as &$entity) {
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
