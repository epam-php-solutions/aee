<?php

class DeployExtraJSON implements DeployExtraFormat {
  public function encodeData($data) {
    return drupal_json_encode($data);
  }

  public function decodeData($data) {
    return drupal_json_decode($data);
  }

  public function configExportForm(&$form_state) {
    return array();
  }

  public function configImportForm(&$form_state) {
    return array();
  }
}
