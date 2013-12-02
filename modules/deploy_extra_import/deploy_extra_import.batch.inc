<?php

/**
 * @file
 * This is the file description for Deploy Extra Import batch.
 */

/**
 * Batch finish import operation.
 *
 * @param $success
 *   Status of batch.
 *
 * @param $results
 *   Batch results
 *
 * @param $operations
 *   Batch operations.
 */
function deploy_extra_import_batch_finished_operation($success, $results, $operations) {
  // Use in translation context.
  $context = array('context' => DEPLOY_EXTRA_IMPORT_TRANSLATE_CONTEXT);

  $status  = $success ? 'status' : 'error';
  $message = $success ? 'Import of entities has done successfully!' : 'Import of entities has failed';

  // Send the message.
  drupal_set_message(t($message, array(), $context), $status);
}
