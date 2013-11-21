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
function hook_deploy_extra_iex_entity_alter(&$entity) {

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
 *   - "label": (required) The label for the method, displayed in the drop down menu.
 *   - "export_callback": (required) The function to call for the export operation.
 *   - "import_callback": (required) The function to call for the import operation.
 *   - "file": (optional) The path to the directory containing the file specified in "file".
 *     This defaults to the path to the module implementing the hook.
 *   - "path": (optional) The directory path of the theme or module, so that it doesn't need to be looked up.
 */
function hook_deploy_extra_iex_methods() {
  $methods = array(
    'form' => array(
      'label' => 'Push to form',
      'export_callback' => '',
      'import_callback' => '',
      'file' => '',
      'path' => '',
    ),
    'file' => array(
      'label' => 'Push to destination on server folder',
      'export_callback' => '',
      'import_callback' => '',
      'file' => '',
      'path' => '',
    ),
    'file_for_downloading' => array(
      'label' => 'Push to browser for downloading',
      'export_callback' => '',
      'import_callback' => '',
      'file' => '',
      'path' => '',
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
