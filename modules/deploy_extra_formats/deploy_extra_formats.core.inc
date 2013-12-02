<?php

/**
 * @file
 * This is the file description for Deploy Extra Formats core.
 */

/**
 * Return plugins configuration.
 *
 * @param null $id
 *   Format ID (DeployExtraJSON, DeployExtraYaml & etc.)
 *
 * @return array
 */
function deploy_extra_formats_get_formats_plugins($id = NULL) {
  ctools_include('plugins');
  return ctools_get_plugins('deploy_extra_formats', 'formats', $id);
}

/**
 * Return export formats options.
 */
function deploy_extra_formats_get_formats_options($form_state, $method) {
  // Get plugins.
  $formats = deploy_extra_formats_get_formats_plugins();
  $options = array();

  // Fill the options array.
  if (!empty($formats)) {
    foreach($formats as $format) {
      $options['plugins_formats'][$format['handler']['class']] = $format['name'];

      // Initialize the plugin.
      $plugin = new $format['handler']['class'];

      // Collect the plugins form configurations.
      $options['plugins_forms'] = $plugin->$method($form_state);
    }
  }

  return $options;
}

/**
 * Return methods options.
 *
 * @param $label
 *   Export or import label.
 *
 * @return array
 */
function deploy_extra_formats_get_methods_options($label) {
  $methods = array();
  foreach (module_invoke_all('deploy_extra_formats_methods') as $method => $array) {
    if ($array[$label] != 'none') {
      $methods[$method] = $array[$label];
    }
  }

  return $methods;
}
