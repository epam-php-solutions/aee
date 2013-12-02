<?php

/**
 * @file
 * Includes the admin callbacks and functions for Deploy Extra Export module.
 */

/**
 * Export form.
 *   Get data from cache and put them to form.
 */
function deploy_extra_export_entity_export_form($form, $form_state, $uuid = NULL) {
  $cache = deploy_extra_export_get_cache($uuid);
  $form['export'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#default_value' => !empty($cache->data) ? $cache->data : '',
  );

  return $form;
}

/**
 * Ajax callback for output the data for downloading.
 *
 * @param $uuid
 *   Deployment key.
 *
 * @param $filename
 *   Filename for downloading.
 */
function deploy_extra_export_ajax_deploy_download($uuid, $filename) {
  $cache = deploy_extra_export_get_cache($uuid);
  $content = $cache->data;

  // Send headers.
  header('Content-Description: File Transfer');
  header('Content-Type:text/plain; charset=utf-8');
  header('Content-Disposition: attachment; filename="' . $filename);
  header('Content-Length: ' . strlen($content));

  // Send data.
  echo $content;
}

/**
 * Gets cache data of entities.
 *   Returns cache data and clears the cache data in DB.
 *
 * @param $uuid
 * @return mixed
 */
function deploy_extra_export_get_cache($uuid) {
  $cid   = 'deploy_extra_export_' . $uuid;
  $cache = cache_get($cid);
  cache_clear_all($uuid, 'cache');

  return $cache;
}
