<?php

/**
 * @file
 * Deploy Extra Log views field handlers.
 */

/**
 * Process status field from integer to string.
 */
class deploy_extra_log_handler_status_field extends views_handler_field {

  function render($values) {
    $status_id = $values->{$this->field_alias};
    $status = _deploy_extra_log_status_to_string($status_id);
    $value = $status[$status_id];
    return $value;
  }
}

/**
 * Field handler to present the path to the entity.
 */

class deploy_extra_log_handler_field_entity_link extends views_handler_field {

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function construct() {
    parent::construct();
    $this->additional_fields['entity_uuid'] = 'entity_uuid';
    $this->additional_fields['entity_type'] = 'entity_type';
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $entity_uuid = $this->get_value($values, 'entity_uuid');
    $entity_type = $this->get_value($values, 'entity_type');

    $ids = entity_get_id_by_uuid($entity_type, array($entity_uuid));

    if (!empty($ids)) {
      $id = reset($ids);
      $entity = entity_load_single($entity_type, $id);
      if (entity_access('view', $entity_type, $entity)) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = "$entity_type/$id";
        $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
        return $text;
      }
    }
  }
}

/**
 * Filter by entity type.
 */
class deploy_extra_log_handler_filter_entity_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Entity types');
      $types = entity_get_info();
      $options = array();
      foreach ($types as $type => $info) {
        $options[$type] = t($info['label']);
      }
      unset($options['taxonomy_vocabulary']);
      asort($options);
      $this->value_options = $options;
    }
  }
}

/**
 * Filter by status field.
 */
class deploy_extra_log_handler_filter_status_field extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Status');
      $types = _deploy_extra_log_status_to_string();
      $options = array();
      foreach ($types as $status_id => $status) {
        $options[$status_id] = $status;
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}

/**
 * Get message from data field.
 */
class deploy_extra_log_handler_data_message extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['data'] = 'data';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {

    $data = $this->get_value($values, 'data');

    if (!empty($data)) {
      $data = unserialize($data);
      if (isset($data['message']) && !empty($data['message'])) {
        $text = deploy_extra_log_message_to_text($data['message']);
      }
      return $text;
    }
  }
}

