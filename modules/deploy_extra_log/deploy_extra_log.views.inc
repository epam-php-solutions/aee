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
  // @TODO add field data and handler to view

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

/**
 * Implements hook_views_default_views().
 * Add a default view of Deploy Extra Log
 */
function deploy_extra_log_views_default_views() {

  $view = new view();
  $view->name = 'deploy_extra_log';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'deploy_extra_import_log';
  $view->human_name = 'Deploy Extra Log';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Deploy Extra Log';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer deployments';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'lid' => 'lid',
    'deployment_uuid' => 'deployment_uuid',
    'entity_uuid' => 'entity_uuid',
    'entity_vuuid' => 'entity_vuuid',
    'entity_type' => 'entity_type',
    'status' => 'status',
    'timestamp' => 'timestamp',
    'message' => 'message',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'lid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'deployment_uuid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'entity_uuid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'entity_vuuid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'entity_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'timestamp' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'message' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Deploy Extra: Log ID */
  $handler->display->display_options['fields']['lid']['id'] = 'lid';
  $handler->display->display_options['fields']['lid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['lid']['field'] = 'lid';
  /* Field: Deploy Extra: Deployment UUID */
  $handler->display->display_options['fields']['deployment_uuid']['id'] = 'deployment_uuid';
  $handler->display->display_options['fields']['deployment_uuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['deployment_uuid']['field'] = 'deployment_uuid';
  /* Field: Deploy Extra: Entity UUID */
  $handler->display->display_options['fields']['entity_uuid']['id'] = 'entity_uuid';
  $handler->display->display_options['fields']['entity_uuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['entity_uuid']['field'] = 'entity_uuid';
  /* Field: Deploy Extra: Entity VUUID */
  $handler->display->display_options['fields']['entity_vuuid']['id'] = 'entity_vuuid';
  $handler->display->display_options['fields']['entity_vuuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['entity_vuuid']['field'] = 'entity_vuuid';
  /* Field: Deploy Extra: Entity Type */
  $handler->display->display_options['fields']['entity_type']['id'] = 'entity_type';
  $handler->display->display_options['fields']['entity_type']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['entity_type']['field'] = 'entity_type';
  /* Field: Deploy Extra: Import status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  /* Field: Deploy Extra: Import time */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'long';
  /* Field: Deploy Extra: Log Message */
  $handler->display->display_options['fields']['message']['id'] = 'message';
  $handler->display->display_options['fields']['message']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['fields']['message']['field'] = 'message';
  /* Filter criterion: Deploy Extra: Deployment UUID */
  $handler->display->display_options['filters']['deployment_uuid']['id'] = 'deployment_uuid';
  $handler->display->display_options['filters']['deployment_uuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['deployment_uuid']['field'] = 'deployment_uuid';
  $handler->display->display_options['filters']['deployment_uuid']['group'] = 1;
  $handler->display->display_options['filters']['deployment_uuid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['deployment_uuid']['expose']['operator_id'] = 'deployment_uuid_op';
  $handler->display->display_options['filters']['deployment_uuid']['expose']['label'] = 'Deployment UUID';
  $handler->display->display_options['filters']['deployment_uuid']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['deployment_uuid']['expose']['operator'] = 'deployment_uuid_op';
  $handler->display->display_options['filters']['deployment_uuid']['expose']['identifier'] = 'deployment_uuid';
  $handler->display->display_options['filters']['deployment_uuid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );
  /* Filter criterion: Deploy Extra: Entity UUID */
  $handler->display->display_options['filters']['entity_uuid']['id'] = 'entity_uuid';
  $handler->display->display_options['filters']['entity_uuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['entity_uuid']['field'] = 'entity_uuid';
  $handler->display->display_options['filters']['entity_uuid']['group'] = 1;
  $handler->display->display_options['filters']['entity_uuid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['entity_uuid']['expose']['operator_id'] = 'entity_uuid_op';
  $handler->display->display_options['filters']['entity_uuid']['expose']['label'] = 'Entity UUID';
  $handler->display->display_options['filters']['entity_uuid']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['entity_uuid']['expose']['operator'] = 'entity_uuid_op';
  $handler->display->display_options['filters']['entity_uuid']['expose']['identifier'] = 'entity_uuid';
  $handler->display->display_options['filters']['entity_uuid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );
  /* Filter criterion: Deploy Extra: Entity VUUID */
  $handler->display->display_options['filters']['entity_vuuid']['id'] = 'entity_vuuid';
  $handler->display->display_options['filters']['entity_vuuid']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['entity_vuuid']['field'] = 'entity_vuuid';
  $handler->display->display_options['filters']['entity_vuuid']['group'] = 1;
  $handler->display->display_options['filters']['entity_vuuid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['entity_vuuid']['expose']['operator_id'] = 'entity_vuuid_op';
  $handler->display->display_options['filters']['entity_vuuid']['expose']['label'] = 'Entity VUUID';
  $handler->display->display_options['filters']['entity_vuuid']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['entity_vuuid']['expose']['operator'] = 'entity_vuuid_op';
  $handler->display->display_options['filters']['entity_vuuid']['expose']['identifier'] = 'entity_vuuid';
  $handler->display->display_options['filters']['entity_vuuid']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );
  /* Filter criterion: Deploy Extra: Entity Type */
  $handler->display->display_options['filters']['entity_type']['id'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['entity_type']['field'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['group'] = 1;
  $handler->display->display_options['filters']['entity_type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['entity_type']['expose']['operator_id'] = 'entity_type_op';
  $handler->display->display_options['filters']['entity_type']['expose']['label'] = 'Entity Type';
  $handler->display->display_options['filters']['entity_type']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['entity_type']['expose']['operator'] = 'entity_type_op';
  $handler->display->display_options['filters']['entity_type']['expose']['identifier'] = 'entity_type';
  $handler->display->display_options['filters']['entity_type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );
  /* Filter criterion: Deploy Extra: Import status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['exposed'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator_id'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['label'] = 'Import status';
  $handler->display->display_options['filters']['status']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['status']['expose']['operator'] = 'status_op';
  $handler->display->display_options['filters']['status']['expose']['identifier'] = 'status';
  $handler->display->display_options['filters']['status']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );
  /* Filter criterion: Deploy Extra: Import time */
  $handler->display->display_options['filters']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['table'] = 'deploy_extra_import_log';
  $handler->display->display_options['filters']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['group'] = 1;
  $handler->display->display_options['filters']['timestamp']['exposed'] = TRUE;
  $handler->display->display_options['filters']['timestamp']['expose']['operator_id'] = 'timestamp_op';
  $handler->display->display_options['filters']['timestamp']['expose']['label'] = 'Import time';
  $handler->display->display_options['filters']['timestamp']['expose']['use_operator'] = TRUE;
  $handler->display->display_options['filters']['timestamp']['expose']['operator'] = 'timestamp_op';
  $handler->display->display_options['filters']['timestamp']['expose']['identifier'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    11 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/structure/deploy/deploy-extra-log';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Deploy Extra Log';
  $handler->display->display_options['menu']['weight'] = '100';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  $views[$view->name] = $view;
  return $views;
}