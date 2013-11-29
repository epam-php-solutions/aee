<?php

/**
 * @file
 * Hooks provided by the Deploy Extra IEX module.
 */

/**
 * Allow modules to modify an entity before it gets exported.
 *
 * @param $entity
 *   The entity being exported.
 */
// @TODO move to deploy_extra api.
function hook_deploy_extra_export_entity_alter(&$entity) {

}

/**
 * Add export methods.
 *
 * This hook add export methods to settings form for DeployPorocessorForm and also
 * import methods to settings form at admin/structure/deploy/import. The callback functions response
 * import and export operations.
 *
 * @return array
 *   An array of methods. Each operation is an associative array that may
 *   contain the following key-value pairs:
 *   - "export_label": (required) The label for the method, displayed in the drop down menu.
 *   - "import_label": (required) The label for the method, displayed in the drop down menu.
 *   - "export_callback": (required) The function to call for the export operation.
 *   - "import_callback": (required) The function to call for the import operation.
 *   If you use "none" in export or import label these labels will not display.
 */
function hook_deploy_extra_iex_methods() {
  $methods = array(
    'form' => array(
      'export_label' => 'Push to form',
      'import_label' => 'Import from form',
      'export_callback' => 'example_form_export_function',
      'import_callback' => 'example_form_import_function',
    ),
    'file' => array(
      'export_label' => 'Push to destination on server folder',
      'import_label' => 'Import from file',
      'export_callback' => 'example_file_export_function',
      'import_callback' => 'example_file_import_function',
    ),
    'file_for_downloading' => array(
      'export_label' => 'Push to browser for downloading',
      'import_label' => 'none',
      'export_callback' => 'example_file_downloading_export_function',
      'import_callback' => 'example_file_downloading_import_function',
    ),
  );

  return $methods;
}

/**
 * Allow modules to modify export|import methods.
 *
 * @param $methods
 *    The associative array include settings of methods.
 */
function hook_deploy_extra_iex_methods_alter(&$methods) {

}

/**
 * Allow modules add own export formats plugins.
 */
function hook_deploy_extra_iex_export_formats() {
  // Set the path where are formats plugins.
  $path = drupal_get_path('module', 'deploy_extra_iex') . '/plugins/export_formats';
  return array(
    'OwnPluginName' => array(
      'name' => 'Own plugin name',
      'description' => 'Provides new export format.',
      'handler' => array(
        'class' => 'OwnPluginName',
        'file'  => 'OwnPluginName.inc',
        'path'  => $path,
      ),
    ),
  );
}
