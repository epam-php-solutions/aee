<?php

/**
 * The interface for export format.
 */
interface DeployExtraFormat {

  /**
   * Encode method.
   *
   * @param $data
   *   Data will be encoded.
   *
   * @return mixed
   */
  public function encodeData($data);

  /**
   * Decode method.
   *
   * @param $data
   *    Data will be decoded.
   *
   * @return mixed
   */
  public function decodeData($data);

  /**
   * Configuration export form.
   *
   * @param array $form_state
   *   The complete form state.
   */
  public function configExportForm(&$form_state);

  /**
   * Configuration import form.
   *
   * @param array $form_state
   *   The complete form state.
   */
  public function configImportForm(&$form_state);

}
