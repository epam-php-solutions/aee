<?php

interface DeployExtraExportFormat {
  /**
   * @param array $config
   */
  public function __construct($config = array());

  /**
   * @param $data
   * @return mixed
   */
  public function encodeData($data);

  /**
   * @param $data
   * @return mixed
   */
  public function decodeData($data);
}
