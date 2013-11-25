<?php

class DeployExtraJSON implements DeployExtraFormat {

  public function __construct($config = array()) {
    return $config;
  }

  public function encodeData($data) {
    return drupal_json_encode($data);
  }

  public function decodeData($data) {
    return drupal_json_decode($data);
  }
}
