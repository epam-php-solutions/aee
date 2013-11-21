<?php
/**
 * @file
 * Provide views data and handlers for deploy_extra_log.module
 */

/**
 * Implements hook_views_data().
 */
function deploy_extra_log_views_data() {
  // Basic table information.

  $data['deploy_extra_import_log']['table']['group']  = t('Deploy Extra');

  // Advertise this table as a possible base table
  $data['deploy_extra_import_log']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Deploy Extra'),
    'help' => t('Deploy Extra Import Log.'),
    'weight' => 10,
  );

  // Log ID
  $data['deploy_extra_import_log']['lid'] = array(
    'title' => t('Log ID'),
    'help' => t('The ID of the Log record.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Log record time when the entity was imported
  $data['deploy_extra_import_log']['timestamp'] = array(
    'title' => t('Import time'),
    'help' => t('The time the entity was imported.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Deployment UUID
  $data['deploy_extra_import_log']['deployment_uuid'] = array(
    'title' => t('Deployment UUID'),
    'help' => t('UUID reference to a deployment.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Entity UUID
  $data['deploy_extra_import_log']['entity_uuid'] = array(
    'title' => t('Entity UUID'),
    'help' => t('UUID of Import Entity'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Entity Revision UUID
  $data['deploy_extra_import_log']['entity_vuuid'] = array(
    'title' => t('Entity VUUID'),
    'help' => t('Revision UUID of Import Entity'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Entity Type
  $data['deploy_extra_import_log']['entity_type'] = array(
    'title' => t('Entity Type'),
    'help' => t('Type of Import Entity'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Status
  $data['deploy_extra_import_log']['status'] = array(
    'title' => t('Import status'),
    'help' => t('A status code for the import entity.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Log Message
  $data['deploy_extra_import_log']['message'] = array(
    'title' => t('Log Message'),
    'help' => t('Complementary message, e.g. the exception in case of a failure.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  return $data;
}