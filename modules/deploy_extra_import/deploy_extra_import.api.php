<?php

/**
 * @file
 * Hooks provided by the Deploy Extra Import module.
 */

/**
 * Add import methods.
 *
 * This hook add import methods to settings form at admin/structure/deploy/import.
 * The callback functions response import operations.
 *
 * @return array
 *   An array of methods. Each operation is an associative array that may
 *   contain the following key-value pairs:
 *   - "import_label": (required) The label for the method, displayed in the drop down menu.
 *   - "import_callback": (required) The function to call for the import operation.
 *   Keys in this array must be same for identically array of export in deploy_extra_export module.
// *   If you use "none" in import label these labels will not display.
 */
function hook_deploy_extra_import_methods() {
  $methods = array(
    'form' => array(
      'import_label' => 'Import from form',
      'import_callback' => 'example_form_import_function',
    ),
    'file' => array(
      'import_label' => 'Import from file',
      'import_callback' => 'example_file_import_function',
    ),
    'file_for_downloading' => array(
      'import_label' => 'Import from path on server',
      'import_callback' => 'example_file_downloading_import_function',
    ),
  );

  return $methods;
}

/**
 * Allow modules to modify import methods.
 *
 * @param $methods
 *    The associative array include settings of methods.
 */
function hook_deploy_extra_import_methods_alter(&$methods) {

}
