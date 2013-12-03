<?php

/**
 * @file
 * Contains DeployExtraYaml class.
 */

class DeployExtraYaml implements DeployExtraFormat {
  public function encodeData($data) {
    $this->loadSpyc();
    foreach ($data['entities'] as &$entity) {
      $entity = (array) $entity;
    }
    unset($entity);
    return Spyc::YAMLDump($data);
  }

  public function decodeData($data) {
    $this->loadSpyc();
    return Spyc::YAMLLoad($data);
  }

  public function configExportForm(&$form_state) {
    return array();
  }

  public function configImportForm(&$form_state) {
    return array();
  }

  /**
   * Load the spyc library.
   */
  private function loadSpyc() {
    libraries_load('spyc');
  }
}
